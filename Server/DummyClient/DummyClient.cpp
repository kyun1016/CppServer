#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

int main()
{
	SetConsoleOutputCP(CP_UTF8);

	// ---------------------------
	// 소켓 생성

	// 윈속 초기화 (ws2_32 라이브러리 초기화)
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return -1;   // 시작이 안되는 경우 오류로 

	// type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
	SOCKET clientSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return -1;
	}


	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777); // 80 : HTTP

	// Connected UDP
	::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	while (true)
	{
		char sendBuffer[100] = "Hello World";

		// 자동으로 나의 IP 주소 + 포트 번호 설정
		//// Unconnected UDP
		//int32 resultCode = ::sendto(clientSocket, sendBuffer, sizeof(sendBuffer), 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

		// Connected UDP
		int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
		if (resultCode == SOCKET_ERROR)
		{
			HandleError("Send To");
			return 0;
		}

		cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;


		// ---------------------------
		// 수신
		SOCKADDR_IN recvAddr; // IPv4
		::memset(&recvAddr, 0, sizeof(recvAddr));
		int32 addrLen = sizeof(recvAddr);

		char recvBuffer[1000];

		// // Unconnected UDP
		// int32 recvLen = ::recvfrom(clientSocket, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&recvAddr, &addrLen);
		// Connected UDP
		int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (recvLen <= 0)
		{
			HandleError("RecvFrom");		// TODO: 추후 멀티쓰레드에서는 오류가 발생한 통신만 끊어주는 방식으로 변경 필요
			// return -1;
		}


		cout << "Recv Data! Data = " << recvBuffer << endl;
		cout << "Recv Data! Len = " << recvLen << endl;

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