#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"

#include <WinSock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


int main()
{
    // ---------------------------
    // ���� ����

    // ���� �ʱ�ȭ (ws2_32 ���̺귯�� �ʱ�ȭ)
    // ���� ������ wsaData�� ä����
    WSADATA wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return -1;   // ������ �ȵǴ� ��� ������ 

    // ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
    // type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
    // protocol : 0
    // return : descriptor
    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Socket ErrorCode : " << errCode << endl;
        return -1;
    }

    // ---------------------------
    // ������ ����
    // ���� �ּҴ�? (IP�ּ� + Port)->XX ����Ʈ YY ȣ
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // �˾Ƽ� IP ������
    serverAddr.sin_port = ::htons(7777); // 80 : HTTP

    // bind
    if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Bind ErrorCode : " << errCode << endl;
        return 0;
    }

    // listen ����
    if (::listen(listenSocket, 10) == SOCKET_ERROR)     // backlog (��� queue)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Listen ErrorCode : " << errCode << endl;
        return 0;
    }

    // ---------------------------
    // ���� �Ϸ�! �������� ������ �ۼ��� ����!

    while (true)
    {
        SOCKADDR_IN clientAddr; // IPv4
        ::memset(&clientAddr, 0, sizeof(clientAddr));
        int32 addrLen = sizeof(clientAddr);

        // �ܸ��� (��� �� Ȱ��)
        SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            int32 errCode = ::WSAGetLastError();
            cout << "Accept ErrorCode : " << errCode << endl;
            return 0;
        }

        // �մ� ����!
        char ipAddress[16];
        ::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
        cout << "Client Connected! IP = " << ipAddress << endl;

        // TODO
    }


    // ---------------------------
    // ���� ���� �� ������ cleaning
    // ���� ���ҽ� ��ȯ
    ::closesocket(listenSocket);

    // ���� ����
    ::WSACleanup();
	return 0;
}