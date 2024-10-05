#include "pch.h"
#include "Listener.h"

/*-------------------
* Listener
-------------------*/

Listener::~Listener()
{
    SocketUtils::Close(_socket);

    for (AcceptEvent* acceptEvent : _acceptEvents)
    {
        xdelete(acceptEvent);
    }
}

bool Listener::StartAccept(NetAddress netAddress)
{
    _socket = SocketUtils::CreateSocket();
    if (_socket == INVALID_SOCKET)
        return false;

    if (GlocpCore.Register(this) == false)
        return false;

    if (SocketUtils::SetReuseAddress(_socket, true) == false)
        return false;

    if (SocketUtils::SetLinger(_socket, 0, 0) == false)
        return false;

    if (SocketUtils::Listen(_socket) == false)
        return false;

    // 실제 유저가 많아지는 경우 accept를 하지 못하는 경우가 생긴다.
    const int32 acceptCount = 1;
    for (int32 i = 0; i < acceptCount; ++i)
    {
        AcceptEvent* acceptEvent = xnew<AcceptEvent>();
        _acceptEvents.push_back(acceptEvent);
        RegisterAccept(acceptEvent);
    }

    return false;
}

void Listener::CloseSocket()
{
    SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
    return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
    ASSERT_CRASH(iocpEvent->GetType() == EventType::Accept);

    AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
    ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
    Session* session = xnew<Session>();

    acceptEvent->Init();
    acceptEvent->SetSession(session);

    DWORD bytesReceived = 0;
    if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
    {
        const int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            // 일단 다시 Accept 걸어준다.
            RegisterAccept(acceptEvent);
        }
    }
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
    Session* session = acceptEvent->GetSession();

    if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
    {
        RegisterAccept(acceptEvent);
        return;
    }

    SOCKADDR_IN sockAddress;
    int32 sizeOfSockAddr = sizeof(sockAddress);
    if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
    {
        RegisterAccept(acceptEvent);
        return;
    }

    session->SetNetAddress(NetAddress(sockAddress));

    cout << "Client Connected!" << endl;

    RegisterAccept(acceptEvent);
}
