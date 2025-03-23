// WebSocketDLL.cpp
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

SOCKET g_socket = INVALID_SOCKET;

extern "C" __declspec(dllexport) int Connect(const char* url, const char* port) {
    WSADATA wsaData;
    struct addrinfo* result = NULL, hints;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(url, port, &hints, &result) != 0)
        return 0;

    g_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (g_socket == INVALID_SOCKET) return 0;

    if (connect(g_socket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        closesocket(g_socket);
        g_socket = INVALID_SOCKET;
        return 0;
    }
    freeaddrinfo(result);
    return 1;
}

extern "C" __declspec(dllexport) int Send(const char* msg) {
    if (g_socket == INVALID_SOCKET) return 0;
    int sent = send(g_socket, msg, (int)strlen(msg), 0);
    return (sent == SOCKET_ERROR) ? 0 : 1;
}

extern "C" __declspec(dllexport) int Receive(char* buffer, int size) {
    if (g_socket == INVALID_SOCKET) return 0;
    int received = recv(g_socket, buffer, size - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        return 1;
    }
    return 0;
}

extern "C" __declspec(dllexport) void Close() {
    if (g_socket != INVALID_SOCKET) {
        closesocket(g_socket);
        g_socket = INVALID_SOCKET;
        WSACleanup();
    }
}
