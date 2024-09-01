#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
    // ---------------------------
    // 소켓 생성
  
    // 윈속 초기화 (ws2_32 라이브러리 초기화)
    // 관련 정보가 wsaData에 채워짐
    WSADATA wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return -1;   // 시작이 안되는 경우 오류로 

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
    // 데이터 연결
    // 연결할 목적지는? (IP주소 + Port) -> XX 아파트 YY 호
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    //serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1"); << deprecated
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = ::htons(7777); // 80 : HTTP
    // host to network short
    // Little-Endian vs Big-Endian (이 처리를 위해 한번 함수를 거쳐준다.)
    
    if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Connect ErrorCode : " << errCode << endl;
        return -1;
    }

    // ---------------------------
    // 연결 성공! 이제부터 데이터 송수신 가능!
    cout << "Connected To Server!" << endl;

    while (true)
    {
        // ---------------------------
        // 송신
        char sendBuffer[100] = "Hello World!";

        for (int32 i = 0; i < 11; i++)
        {
            int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
            if (resultCode == SOCKET_ERROR)
            {
                int32 errCode = ::WSAGetLastError();
                cout << "Send ErrorCode : " << errCode << endl;
                return 0;
            }
        }

        cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;

        //// ---------------------------
        //// 수신
        //char recvBuffer[1000];

        //this_thread::sleep_for(1s);

        //int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        //if (recvLen <= 0)
        //{
        //    int32 errCode = ::WSAGetLastError();
        //    cout << "Recv ErrorCode : " << errCode << endl;
        //    return 0;
        //}

        //cout << "Recv Data! Data = " << recvBuffer << endl;
        //cout << "Recv Data! Len = " << recvLen << endl;


        this_thread::sleep_for(1s);
    }

    // ---------------------------
    // 동작 종료 후 데이터 cleaning
    // 소켓 리소스 반환
    ::closesocket(clientSocket);

    // 윈속 종료
    ::WSACleanup();

    return 0;
}