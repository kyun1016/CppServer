#include "pch.h"
#include <iostream>

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
    SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Socket ErrorCode : " << errCode << endl;
        return -1;
    }

    // ---------------------------
    // ������ ����
    // ������ ��������? (IP�ּ� + Port) -> XX ����Ʈ YY ȣ
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    //serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1"); << deprecated
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = ::htons(7777); // 80 : HTTP
    // host to network short
    // Little-Endian vs Big-Endian (�� ó���� ���� �ѹ� �Լ��� �����ش�.)
    
    if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Connect ErrorCode : " << errCode << endl;
        return -1;
    }

    // ---------------------------
    // ���� ����! �������� ������ �ۼ��� ����!
    cout << "Connected To Server!" << endl;

    while (true)
    {
        // TODO

        this_thread::sleep_for(1s);
    }

    // ---------------------------
    // ���� ���� �� ������ cleaning
    // ���� ���ҽ� ��ȯ
    ::closesocket(clientSocket);

    // ���� ����
    ::WSACleanup();

    return 0;
}