#include "pch.h"
#include "IocpCore.h"

// TEMP
IocpCore GlocpCore;

IocpCore::IocpCore()
{
    _iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
    ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
    ::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(class IocpObject* iocpObject)
{
    // ����� �Ϸ� ��Ʈ�� �����, ������ ���� �ڵ鿡 �����ϰų�, ���� ���� �ڵ鿡 ������� ���� ����� �Ϸ� ��Ʈ�� ����� ���߿� ������ �� �ִ�.
    // return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, reinterpret_cast<ULONG_PTR>(iocpObject), 0);
    return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*key*/0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
    DWORD numOfBytes = 0;
    ULONG_PTR key = 0;
    IocpObject* iocpObject = nullptr;
    IocpEvent* iocpEvent = nullptr;

    // if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT reinterpret_cast<PULONG_PTR>(&iocpObject), OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
    if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT &key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
    {
        iocpObject->Dispatch(iocpEvent, numOfBytes);
    }
    else
    {
        int32 errCode = ::WSAGetLastError();
        switch (errCode)
        {
        case WAIT_TIMEOUT:
            return false;
        default:
            // TODO: �α� ���
            break;
        }
    }
    return true;
}
