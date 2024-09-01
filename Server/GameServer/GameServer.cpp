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

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}
int main()
{
	// ---------------------------
	// 소켓 생성

	// 윈속 초기화 (ws2_32 라이브러리 초기화)
	// 관련 정보가 wsaData에 채워짐
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return -1;   // 시작이 안되는 경우 오류로 

	SOCKET serverSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return -1;
	}

	// ---------------------------
	// bind
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // 알아서 IP 맞춰줘
	serverAddr.sin_port = ::htons(7777); // 80 : HTTP


	if (::bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		HandleError("Bind");
		return -1;
	}

	// ---------------------------
	// 연결 완료! 이제부터 데이터 송수신 가능!

	while (true)
	{
		SOCKADDR_IN clientAddr; // IPv4
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 addrLen = sizeof(clientAddr);

		// this_thread::sleep_for(1s);

		char recvBuffer[1000];

		int32 recvLen = ::recvfrom(serverSocket, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&clientAddr, &addrLen);
		if (recvLen <= 0)
		{
			HandleError("RecvFrom");		// TODO: 추후 멀티쓰레드에서는 오류가 발생한 통신만 끊어주는 방식으로 변경 필요
			return -1;
		}

		cout << "Recv Data! Data = " << recvBuffer << endl;
		cout << "Recv Data! Len = " << recvLen << endl;

		int32 errorCode = ::sendto(serverSocket, recvBuffer, recvLen, 0, (SOCKADDR*)&clientAddr, sizeof(clientAddr));

		if (errorCode == SOCKET_ERROR)
		{
			HandleError("Send To");
			return -1;
		}
		cout << "Send Data! Len = " << recvLen << endl;
	}


	// ---------------------------
	// 동작 종료
	// 소켓 리소스 반환
	::closesocket(serverSocket);

	// 윈속 종료
	::WSACleanup();
	return 0;
}