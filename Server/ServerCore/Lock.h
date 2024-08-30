#pragma once
#include "Types.h"

/*-----------------
	RW SpinLock
-----------------*/

/*-----------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : WriteFlag (Exclusive Lock Owner ThreadId
R : ReadFlag (Shared Lock Count)
-----------------*/

// W -> W (O)
// W -> R (O)
// R -> W (X)
class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};
public:
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint16 _writeCount = 0;
};

/*-----------------
	LOCKGuards
-----------------*/

// RAII
class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock, const char* name)
		: _lock(lock)
		, _name(name)
	{
		_lock.ReadLock(_name);
	}

	~ReadLockGuard()
	{
		_lock.ReadUnlock(_name);
	}
private:
	Lock& _lock;
	const char* _name;
};

// RAII
class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name)
		: _lock(lock)
		, _name(name)
	{
		_lock.WriteLock(_name);
	}

	~WriteLockGuard()
	{
		_lock.WriteUnlock(_name);
	}
private:
	Lock& _lock;
	const char* _name;
};