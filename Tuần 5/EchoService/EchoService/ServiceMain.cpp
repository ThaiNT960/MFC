#include "Service.h"

SERVICE_STATUS g_ServiceStatus = {};
SERVICE_STATUS_HANDLE g_StatusHandle = nullptr;
HANDLE g_StopEvent = nullptr;

void ReportServiceStatus(DWORD state, DWORD errorCode)
{
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = state;
    g_ServiceStatus.dwWin32ExitCode = errorCode;
    g_ServiceStatus.dwControlsAccepted =
        (state == SERVICE_RUNNING) ? SERVICE_ACCEPT_STOP : 0;

    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
}

int wmain()
{
    SERVICE_TABLE_ENTRYW serviceTable[] = {
        { const_cast<LPWSTR>(SERVICE_NAME), ServiceMain },
        { nullptr, nullptr }
    };

    if (!StartServiceCtrlDispatcherW(serviceTable))
        return static_cast<int>(GetLastError());

    return 0;
}

void WINAPI ServiceMain(DWORD, LPWSTR*)
{
    g_StatusHandle =
        RegisterServiceCtrlHandlerW(SERVICE_NAME, ServiceCtrlHandler);

    if (g_StatusHandle == nullptr)
        return;

    ReportServiceStatus(SERVICE_START_PENDING);

    g_StopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_StopEvent == nullptr)
    {
        ReportServiceStatus(SERVICE_STOPPED, GetLastError());
        return;
    }
    DWORD serverError = StartTcpServer(g_StopEvent);

    CloseHandle(g_StopEvent);
    g_StopEvent = nullptr;

    ReportServiceStatus(SERVICE_STOPPED, serverError);
}

void WINAPI ServiceCtrlHandler(DWORD control)
{
    if (control == SERVICE_CONTROL_STOP)
    {
        ReportServiceStatus(SERVICE_STOP_PENDING);
        SetEvent(g_StopEvent);
    }
}
