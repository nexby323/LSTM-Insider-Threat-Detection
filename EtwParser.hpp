#pragma once
#include <windows.h>
#include <evntrace.h>
#include <string>
#include <vector>

class EtwParser {
public:
    static std::wstring GetEventProperty(PEVENT_RECORD pEvent, LPCWSTR propertyName);
};