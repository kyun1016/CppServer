#include "pch.h"
#include "IocpEvent.h"

IocpEvent::IocpEvent(EventType type)
    : _type(type)
{
    Init();
}

void IocpEvent::Init()
{
    /*
    typedef struct _OVERLAPPED {
        ULONG_PTR Internal;
        ULONG_PTR InternalHigh;
        union {
            struct {
                DWORD Offset;
                DWORD OffsetHigh;
            } DUMMYSTRUCTNAME;
            PVOID Pointer;
        } DUMMYUNIONNAME;

        HANDLE  hEvent;
    } OVERLAPPED, *LPOVERLAPPED;
    */
    OVERLAPPED::hEvent = 0;
    OVERLAPPED::Internal = 0;
    OVERLAPPED::InternalHigh = 0;
    OVERLAPPED::Offset = 0;
    OVERLAPPED::OffsetHigh = 0;
}
