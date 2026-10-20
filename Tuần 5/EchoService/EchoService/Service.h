#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define SERVICE_NAME L"EchoService"

void WINAPI ServiceMain(DWORD argc, LPWSTR* argv);
void WINAPI ServiceCtrlHandler(DWORD control);

void ReportServiceStatus(DWORD state, DWORD errorCode = NO_ERROR);
DWORD StartTcpServer(HANDLE stopEvent);