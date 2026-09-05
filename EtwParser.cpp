#include "EtwParser.hpp"
#include <tdh.h> //https://learn.microsoft.com/en-us/windows/win32/api/tdh/ (essential for etw)
//preprocessor directive (comment(lib, "lib_name")) that tells the compiler to leave a comment record in the generated object file
//then automaticly the linker reads this comment and from that links the specified library file to your executable.
#pragma comment(lib, "tdh.lib")

//The method get an event (PEVENT_RECORD) and a property in the event and return the property value in the event
std::wstring EtwParser::GetEventProperty(PEVENT_RECORD pEvent, LPCWSTR propertyName) {
    PROPERTY_DATA_DESCRIPTOR descriptor; //
    //init the obejct above on the safe-way
    ZeroMemory(&descriptor, sizeof(PROPERTY_DATA_DESCRIPTOR));
    //set the feilds of the descriptor as needed 
    descriptor.PropertyName = reinterpret_cast<ULONGLONG>(propertyName);
    //
    descriptor.ArrayIndex = ULONG_MAX;

    DWORD bufferSize = 0;
    
    // https://learn.microsoft.com/en-us/windows/win32/api/tdh/nf-tdh-tdhgetpropertysize get the size of the property variable
    DWORD status = TdhGetPropertySize(pEvent, 0, NULL, 1, &descriptor, &bufferSize);
    if (status != ERROR_SUCCESS || bufferSize == 0) {
        return L"Unknown";
    }

    // https://learn.microsoft.com/en-us/windows/win32/api/tdh/nf-tdh-tdhgetproperty get property value(string)
    std::vector<BYTE> buffer(bufferSize);
    status = TdhGetProperty(pEvent, 0, NULL, 1, &descriptor, bufferSize, buffer.data());
    
    if (status == ERROR_SUCCESS) {
        return std::wstring(reinterpret_cast<wchar_t*>(buffer.data()));
    }
    
    return L"Unknown";
}