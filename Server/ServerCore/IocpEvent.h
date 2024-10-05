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
// �������� �����Լ��� ����ϴ� �Ϳ� �����ؾ��Ѵ�.
// �����Լ��� ����ϸ� Offset 0���� �����Լ� ���̺��� ���鼭
// �Ҹ��� ���ۿ� ������ ���� �� �ִ�.
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