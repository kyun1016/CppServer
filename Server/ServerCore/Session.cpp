#include "pch.h"
#include "Session.h"

/*-------------------
* Session
-------------------*/

Session::Session()
{
    _socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
    SocketUtils::Close(_socket);
}

HANDLE Session::GetHandle()
{
    return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
    // TODO : recv�� send�� ���� �ٸ� ó��

}
