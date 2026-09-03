#include <windows.h>
#include <evntrace.h>
#include <evntcons.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "TcpClient.hpp"
#include "EtwParser.hpp"

std::unordered_map<std::wstring, int> eventVocab;
TcpClient* globalTcpClient = nullptr;

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
    EVENT_TRACE_LOGFILE traceSession;

public:
    EtwSession() : hTrace(INVALID_PROCESSTRACE_HANDLE) {
        ZeroMemory(&traceSession, sizeof(EVENT_TRACE_LOGFILE));
        traceSession.LoggerName = const_cast<LPWSTR>(L"System");
        traceSession.ProcessTraceMode = PROCESS_TRACE_MODE_REAL_TIME | PROCESS_TRACE_MODE_EVENT_RECORD;
        traceSession.EventRecordCallback = ProcessSystemEvent;
        
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