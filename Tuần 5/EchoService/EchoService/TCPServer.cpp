#include "Service.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <new>

#pragma comment(lib, "Ws2_32.lib")

struct ClientContext
{
    SOCKET socket;
    HANDLE stopEvent;
};

DWORD WINAPI ClientHandler(LPVOID parameter)
{
    ClientContext* context = static_cast<ClientContext*>(parameter);
    SOCKET client = context->socket;
    HANDLE stopEvent = context->stopEvent;
    delete context;

    char buffer[1024];

    // Giới hạn thời gian chờ của send để Stop không kẹt mãi
    DWORD sendTimeoutMs = 1000;
    setsockopt(client, SOL_SOCKET, SO_SNDTIMEO,
        reinterpret_cast<const char*>(&sendTimeoutMs),
        sizeof(sendTimeoutMs));

    while (WaitForSingleObject(stopEvent, 0) != WAIT_OBJECT_0)
    {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(client, &readSet);

        timeval timeout = {};
        timeout.tv_usec = 200000; // 200 ms

        int ready = select(0, &readSet, nullptr, nullptr, &timeout);
        if (ready == SOCKET_ERROR)
            break;

        if (ready == 0)
            continue;

        int received = recv(client, buffer, sizeof(buffer), 0);
        if (received <= 0)
            break;

        int sentTotal = 0;
        while (sentTotal < received)
        {
            if (WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0)
                break;

            int sent = send(client, buffer + sentTotal,
                received - sentTotal, 0);
            if (sent <= 0)
                break;

            sentTotal += sent;
        }

        if (sentTotal != received)
            break;

        OutputDebugStringW(L"[SERVICE] Echoed data\n");
    }
    closesocket(client);
    return 0;
}

DWORD StartTcpServer(HANDLE stopEvent)
{
    WSADATA wsa = {};
    int result = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (result != 0)
        return static_cast<DWORD>(result);

    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == INVALID_SOCKET)
    {
        DWORD error = static_cast<DWORD>(WSAGetLastError());
        WSACleanup();
        return error;
    }
    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(9999);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server, reinterpret_cast<sockaddr*>(&address),
        sizeof(address)) == SOCKET_ERROR)
    {
        DWORD error = static_cast<DWORD>(WSAGetLastError());
        closesocket(server);
        WSACleanup();
        return error;
    }

    if (listen(server, SOMAXCONN) == SOCKET_ERROR)
    {
        DWORD error = static_cast<DWORD>(WSAGetLastError());
        closesocket(server);
        WSACleanup();
        return error;
    }
    ReportServiceStatus(SERVICE_RUNNING);
    OutputDebugStringW(L"[SERVICE] Đang lắng nghe TCP trên cổng 9999\n");

    std::vector<HANDLE> clientThreads;
    DWORD serverError = NO_ERROR;

    while (WaitForSingleObject(stopEvent, 0) != WAIT_OBJECT_0)
    {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(server, &readSet);

        timeval timeout = {};
        timeout.tv_usec = 200000; 

        int ready = select(0, &readSet, nullptr, nullptr, &timeout);
        if (ready == SOCKET_ERROR)
        {
            serverError = static_cast<DWORD>(WSAGetLastError());
            break;
        }
        if (ready == 0)
            continue;

        if (WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0)
            break;

        SOCKET client = accept(server, nullptr, nullptr);
        if (client == INVALID_SOCKET)
            continue;

        ClientContext* context = new (std::nothrow) ClientContext{
            client, stopEvent
        };
        if (context == nullptr)
        {
            closesocket(client);
            continue;
        }
        HANDLE thread = CreateThread(
            nullptr, 0, ClientHandler, context, 0, nullptr);

        if (thread == nullptr)
        {
            closesocket(client);
            delete context;
            continue;
        }
        clientThreads.push_back(thread);
        OutputDebugStringW(L"[SERVICE] Đã có client kết nối\n");
    }
    closesocket(server);

    for (HANDLE thread : clientThreads)
    {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }

    WSACleanup();
    return serverError;
}
