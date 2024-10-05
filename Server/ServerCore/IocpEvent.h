#pragma once

class Session;

enum class EventType : uint8
{
    Connect,
    Accept,
    // PreRecv, 0 byte recv
    Recv,
    Send
};

/*-------------------
* IocpEvent
-------------------*/
// 주의점은 가상함수를 사용하는 것에 조심해야한다.
// 가상함수를 사용하며 Offset 0번에 가상함수 테이블이 들어가면서
// 소멸자 동작에 오류가 생길 수 있다.
class IocpEvent : public OVERLAPPED
{
public:
    IocpEvent(EventType type);

    void Init();
    EventType GetType() { return _type; }
protected:
    EventType _type;
    IocpObjectRef owner;
};

/*-------------------
* ConnectEvent
-------------------*/
class ConnectEvent : public IocpEvent
{
public:
    ConnectEvent() : IocpEvent(EventType::Connect) {}
};
/*-------------------
* AcceptEvent
-------------------*/
class AcceptEvent : public IocpEvent
{
public:
    AcceptEvent() : IocpEvent(EventType::Accept) {}

    void SetSession(Session* session) { _session = session; }
    Session* GetSession() { return _session; }
private:
    Session* _session = nullptr;
};
/*-------------------
* ReceiveEvent
-------------------*/
class ReceiveEvent : public IocpEvent
{
public:
    ReceiveEvent() : IocpEvent(EventType::Recv) {}
};
/*-------------------
* SendEvent
-------------------*/
class SendEvent : public IocpEvent
{
public:
    SendEvent() : IocpEvent(EventType::Send) {}
};