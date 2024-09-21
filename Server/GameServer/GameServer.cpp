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

const int32 BUFSIZE = 1000;

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
	int32 sendBytes = 0;
};
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

	// WSAEventSelect = (WSAEventSelect 함수가 핵심이 되는)
	// 소켓과 관련된 네트워크 이벤트를 [이벤트 객체]를 통해 감지

	// 이벤트 객체 관련 함수들
	// 생성 : WSACreateEvent (수동 리셋 Manual-Reset + Non-Signaled 상태 시작)
	// 삭제 : WSACloseEvent
	// 신호 상태 감지 : WSAWaitForMultipleEvents
	// 구체적인 네트워크 이벤트 알아내기 : WSAEventNetworkEvents

	// 소켓 <-> 이벤트 객체 연동
	// WSAEventSelect(socket, event, networkEvents);
	// - 관심있는 네트워크 이벤트
	// FD_ACCEPT : 접속한 클라가 있음 accept
	// FD_READ : 데이터 수신 가능 recv, recvfrom
	// FD_WRITE : 데이터 송신 가능 send, sendto
	// FD_CLOSE : 상대가 접속 종료
	// FD_CONNECT : 통신을 위한 연결 절차 완료
	// FD_OOB

	// 주의사항
	// 1. WSAEventSelect 함수를 호출하면, 해당 소켓은 자동으로 넌블로킹 모드로 저노한됨
	// 2. accept()함수가 리턴하는 소켓은 listenSocket과 동일한 속성을 갖는다.
	// -> 따라서, clientSocket은 FD_READ, FD_WRITE 등을 다시 등록할 필요가 있다.
	// 3. 드물게 WSAEWOULDBLOCK 오류가 뜰 수 있으니 예외 처리 필요
	// 중요)
	// 1. 이벤트 발생 시, 적절한 소켓 함수 호출해야 함
	// 2. 아니면 다음 번에는 동일 네트워크 이벤트가 발생 X
	// 즉, 항상 짝에 맞게 응용을 해야 이벤트가 정상적으로 동작한다.

	// https://learn.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-wsaeventselect
	// https://learn.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-wsawaitformultipleevents
	// WSAWaitForMultifle
	// Input:
	// 1. count, event	이벤트 개체 핸들의 최대 수는 WSA_MAXIMUM_WAIT_EVENTS
	// 2. waitAll : 모두 기다림? 하나만 완료되어도?
	// 3. timeout
	// 4. 스레드가 대기 상태에 자동 배치? 현재는 FALSE 활용
	// Output:
	// 완료된 첫번째 인덱스
	
	// https://learn.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-wsaenumnetworkevents
	// WSAEnumNetworkEvents
	// Input:
	// 1. socket
	// 2. eventObject : socket과 연동된 이벤트 객체 핸들을 넘겨주면, 이벤트 객체를 non-signaled
	// Output:
	// 1. networkEvent : 네트워크 이벤트 / 오류 정보가 저장
	//




	vector<WSAEVENT> wsaEvents;
	vector<Session> sessions;
	wsaEvents.reserve(100);
	sessions.reserve(100);

	WSAEVENT listenEvent = ::WSACreateEvent();
	wsaEvents.push_back(listenEvent);
	sessions.push_back(Session{ listenSocket });	// 실제 클라이언트와 연결된 소켓은 아니지만, wsaEvent와 session의 index를 맞춰주기 위한 방법
	// 두 벡터를 연동
	if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	{
		HandleError("WSAEventSelect Error");
		return -1;
	}

	while (true)
	{
		int32 index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);		// 크기, 포인터, 하나만 완료되도 탈출, 무한정 대기
		if (index == WSA_WAIT_FAILED)
		{
			cout << "*Warn, WSA Wait Fail" << endl;
			continue;
		}
		index -= WSA_WAIT_EVENT_0;

		
		WSANETWORKEVENTS networkEvents;
		if (::WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents) == SOCKET_ERROR)	// ::WSAResetEvent(wsaEvents[index]); 다음 기능이 포함되어 있다.
		{
			cout << "*Warn, WSA Socket Error" << endl;
			continue;
		}
		this_thread::sleep_for(1s);
		// Listener 소켓 체크
		if (networkEvents.lNetworkEvents & FD_ACCEPT)
		{
			// Error-Check
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
			{
				cout << "*Warn, FD ACCEPT Error" << endl;
				continue;
			}

			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket == INVALID_SOCKET)
			{
				HandleError("InValid Socket Error");
				continue;
			}

			cout << "Client Connected" << endl;
			WSAEVENT clientEvent = ::WSACreateEvent();
			wsaEvents.push_back(clientEvent);
			sessions.push_back(Session{ clientSocket });
			// 두 벡터를 연동
			if (::WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
			{
				HandleError("WSAEventSelect Error");
				continue;
			}
		}

		// Client Session 소켓 체크
		if (networkEvents.lNetworkEvents & FD_READ || networkEvents.lNetworkEvents & FD_WRITE)
		{
			// Error-Check
			if ((networkEvents.lNetworkEvents & FD_READ) && (networkEvents.iErrorCode[FD_READ_BIT] != 0))
			{
				cout << "*Warn, FD READ Error" << endl;
				continue;
			}
			if ((networkEvents.lNetworkEvents & FD_WRITE) && (networkEvents.iErrorCode[FD_WRITE_BIT] != 0))
			{
				cout << "*Warn, FD WRITE Error" << endl;
				continue;
			}

			Session& s = sessions[index];

			// Read
			if (s.recvBytes == 0)
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				if (recvLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					// TODO : Remove Session
					continue;
				}

				s.recvBytes = recvLen;
				cout << "Recv Data = " << recvLen << endl;
			}

			// Write
			if (s.recvBytes > s.sendBytes)
			{
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					// TODO : Remove Session
					continue;
				}

				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes)
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}

				cout << "Send Data = " << sendLen << endl;
			}
		}

		// FD_CLOSE 처리
		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			// TODO : Remove Socket
		}
	}

	// 윈속 종료
	::WSACleanup();
	return 0;
}