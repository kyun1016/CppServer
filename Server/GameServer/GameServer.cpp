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

	SOCKET serverSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return -1;
	}

	// 옵션을 해석하고 처리할 주체?
	// 소켓 코드 -> SOL_SOCKET
	// IPv4 -> IPPROTO_IP
	// TCP 프로토콜 -> IPPROTO_TCP
	// SO_KEEPALIVE = 주기적으로 연결 상태 확인 여부 (TCP Only)
	// 상대방이 소리소문 없이 연결을 끊는다면?
	// 주기적으로 연결을 확인하는 방법이다.
	bool enable = true;
	::setsockopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&enable, sizeof(enable));

	// SO_LINGER = 지연하다
	// 송신 버퍼에 있는 데이터를 보낼 것인가? 날릴 것인가?
	//struct  linger {
	//	u_short l_onoff;                /* option on/off */		// onoff = 0이면 closesocket()이 바로 리턴. 아니면 linger초만큼 대기 (default 0)
	//	u_short l_linger;               /* linger time */		// 대기 시간
	//};
	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 5;
	::setsockopt(serverSocket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

	// SO_SNDBUF = 송신 버퍼 크기
	// SO_RCVBUF = 수신 버퍼 크기
	int32 sendBufferSize;
	int32 optionLen = sizeof(sendBufferSize);
	::getsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufferSize, &optionLen);
	cout << "송신 버퍼 크기 : " << sendBufferSize << endl;

	int32 recvBufferSize=0;
	optionLen = sizeof(recvBufferSize);
	::getsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufferSize, &optionLen);
	cout << "수신 버퍼 크기 : " << recvBufferSize << endl;


	// SO_REUSEADDR
	// IP주소 및 port 재사용 (테스트 시 사용 용도)
	{
		bool enable = true;
		::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable));
	}

	// IPPROTO_TCP
	// TCP_NODELAY = Nagle 네이글 알고리즘 작동 여부
	// 데이터가 충분히 크면 보내고, 그렇지 않으면 데이터가 충분히 쌓일때까지 대기!
	// 하지만, 게임에서는 일반적으로 끈다.
	{
		bool enable = true;
		::setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable));
	}

	//// Half-Close
	//// SD_SEND : send 막는다
	//// SD_RECEIVER : recv 막는다
	//// SD_BOTH : 둘다 막는다
	//::shutdown(serverSocket, SD_SEND);

	//// 소켓 리소스 반환 (이후에 전달을 받을 수 없다)
	//// send -> closesocket
	//::closesocket(serverSocket);

	

	// 윈속 종료
	::WSACleanup();
	return 0;
}