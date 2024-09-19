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
	SetConsoleOutputCP(CP_UTF8);

	// ---------------------------
	// 소켓 생성
	// 윈속 초기화 (ws2_32 라이브러리 초기화)
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return -1;   // 시작이 안되는 경우 오류로 

	// 지금까지 방식은 블로킹(Blocking) 소켓
	// accept -> 접속한 클라가 있을 때
	// connect -> 서버 접속 성공했을 때
	// send, sendto -> 요청한 데이터를 송신 버퍼에 복사했을 때
	// recv, recvfrom -> 수신 버퍼에 도착한 데이터가 있고, 이를 유저레벨 버퍼에 복사했을 때

	// Non-Blocking
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		HandleError("Create Socket");
		return -1;
	}

	// https://learn.microsoft.com/ko-kr/windows/win32/api/winsock/nf-winsock-ioctlsocket
	// If on = 0, blocking is enabled;
	// If on != 0, non-blocking mode is enabled.
	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
	{
		HandleError("Setting Non-Blocking Socket");
		return -1;
	}

	bool enable = true;
	::setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable));
	// https://learn.microsoft.com/ko-kr/windows/win32/winsock/windows-sockets-error-codes-2
	// https://learn.microsoft.com/ko-kr/windows/win32/winsock/so-exclusiveaddruse
	// ::setsockopt(listenSocket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&enable, sizeof(enable));

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		HandleError("Bind Addr");
		return -1;
	}

	// https://learn.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-listen
	// SOMAXCONN : 백로그에 적절한 최대값으로 설정
	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		HandleError("Listen Addr");
		return -1;
	}

	cout << "Accept" << endl;

	SOCKADDR_IN clientAddr;
	int32 addrLen = sizeof(clientAddr);

	// Accept
	while (true)
	{
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			// 블록했어야 하지만, Non-Blocking 이여서 pass
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			// Error
			break;
		}

		cout << "Client Connected!" << endl;

		// Recv
		while (true) {
			char recvBuffer[1000];
			int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen == SOCKET_ERROR)
			{
				// 블록했어야 하지만, Non-Blocking 이여서 pass
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
					continue;

				// Error
				break;
			}
			else if (recvLen == 0)
			{
				// 연결 끊김
				break;
			}

			cout << "Recv Data Len = " << recvLen << endl;

			// Send (echo 방식으로 동작)
			while (true)
			{
				if (::send(clientSocket, recvBuffer, recvLen, 0) == SOCKET_ERROR)
				{
					// 블록했어야 하지만, Non-Blocking 이여서 pass
					if (::WSAGetLastError() == WSAEWOULDBLOCK)
						continue;

					// Error
					break;
				}

				cout << "Send Data ! Len = " << recvLen << endl;
				break;
			}
		}
	}


	// 윈속 종료
	::WSACleanup();
	return 0;
}