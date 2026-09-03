#define WIN32_LEAN_AND_MEAN // https://devblogs.microsoft.com/oldnewthing/20091130-00/?p=15863
#include <winsock2.h> //for socketing (networking)
#include <windows.h> //for using windows api to trace the system
#include <evntrace.h>//used for the ETW infastructure see - https://learn.microsoft.com/en-us/windows/win32/api/_etw/
#include <evntcons.h>//used for the ETW infastructure
#include <iostream> //normal I/O operations
#include <fstream> //for loading and writing file
#include <string>//for using strings easly
#include <unordered_map> //the map data-structure
#include "TcpClient.hpp" //the class for communicating with the python server
#include "EtwParser.hpp" //the class for  



//the object to hold mapping of  Source|MachineName pairs to unique integer (id) , (format is Source|MachineName=id)
std::unordered_map<std::wstring, int> eventVocab;
//the bridge to send the server to get the result of the model about the data from the OS
TcpClient* globalTcpClient = nullptr;

//load into to map the vocabulary according to the format (Source|MachineName=id)
void LoadVocabulary() {
    std::ifstream file("event_vocab.txt");
    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "[Agent] Critical Error: Failed to open event_vocab.txt" << std::endl;
        return;
    }

    while (std::getline(file, line)) {
        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string valueStr = line.substr(delimiterPos + 1);
            
            std::wstring wkey(key.begin(), key.end());
            eventVocab[wkey] = std::stoi(valueStr);
        }
    }
    std::cout << "[Agent] Vocabulary instantiated. Size: " << eventVocab.size() << std::endl;
}
//EVENT_RECORD structure https://learn.microsoft.com/en-us/windows/win32/api/evntcons/ns-evntcons-event_record
//get an event - the parameter (PEVENT_RECORD) translate it into the id via the vocabulary and tranfer it to the server  
VOID WINAPI ProcessSystemEvent(PEVENT_RECORD pEventRecord) {

    if (pEventRecord->UserDataLength > 0 && pEventRecord->UserData != NULL) {
        std::wstring machineName = EtwParser::GetEventProperty(pEventRecord, L"MachineName");
        std::wstring source = EtwParser::GetEventProperty(pEventRecord, L"Source");
        
        std::wstring compositeKey = source + L"|" + machineName;

        if (eventVocab.find(compositeKey) != eventVocab.end()) {
            int eventId = eventVocab[compositeKey];
            if (globalTcpClient != nullptr) {
                globalTcpClient->TransmitEvent(eventId);
            }
        }
    }
}

class EtwSession {
private:

    TRACEHANDLE hTrace;

    //https://learn.microsoft.com/en-us/windows/win32/api/evntrace/ns-evntrace-event_trace_logfilea
    //to see all the flag and parameter init in the c'tor of this class
    EVENT_TRACE_LOGFILE traceSession;

public:
    EtwSession() : hTrace(INVALID_PROCESSTRACE_HANDLE) {
        ZeroMemory(&traceSession, sizeof(EVENT_TRACE_LOGFILE));
        traceSession.LoggerName = const_cast<LPTSTR>(TEXT("System"));
        traceSession.ProcessTraceMode = PROCESS_TRACE_MODE_REAL_TIME | PROCESS_TRACE_MODE_EVENT_RECORD;
        traceSession.EventRecordCallback = ProcessSystemEvent;
        https://learn.microsoft.com/en-us/windows/win32/api/evntrace/nf-evntrace-opentracea 
        hTrace = OpenTrace(&traceSession);
        if (hTrace == INVALID_PROCESSTRACE_HANDLE) {
            std::cerr << "Initialization Error: Requires elevated administrator privileges." << std::endl;
        }
    }

    ~EtwSession() {
        if (hTrace != INVALID_PROCESSTRACE_HANDLE) {
            CloseTrace(hTrace);
        }
    }

    void ExecuteMonitor() {
        if (hTrace != INVALID_PROCESSTRACE_HANDLE) {
            std::cout << "Monitoring infrastructure active. Intercepting kernel events..." << std::endl;
            //https://learn.microsoft.com/en-us/windows/win32/api/evntrace/nf-evntrace-processtrace see what the function does 
            ProcessTrace(&hTrace, 1, 0, 0);
        }
    }
};

int main() {
    LoadVocabulary();
    
    TcpClient networkClient("127.0.0.1", 65432);
    globalTcpClient = &networkClient;

    EtwSession agentSession;
    agentSession.ExecuteMonitor();
    
    return 0;
}