#pragma once
#include "IocpEvent.h"

/*-------------------
* IocpObject
-------------------*/
class IocpObject
{
public:
    virtual HANDLE GetHandle() abstract;
    virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;

private:
};

/*-------------------
* IocpCore
-------------------*/

class IocpCore
{
public:
    IocpCore();
    ~IocpCore();

    HANDLE GetHandel() { return _iocpHandle; }

    bool Register(class IocpObject* iocpObject);
    bool Dispatch(uint32 timeoutMs = INFINITE);

private:
    HANDLE _iocpHandle;
};

// TEMP
extern IocpCore GlocpCore;

