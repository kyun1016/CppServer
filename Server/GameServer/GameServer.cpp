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

class Player
{
public:
    Player() {}
    virtual ~Player() {}
};

class Knight : public Player
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}

	//static void* operator new(size_t size)
	//{
	//    cout << "Knight new! " << size << endl;
	//    void* ptr = ::malloc(size);
	//    return ptr;
	//}

	//static void operator delete(void* ptr)
	//{
	//    cout << "Knight delete!" << endl;
	//    ::free(ptr);
	//}

	int64 _hp = 10;
	int32 _damage = 100;
	int32 _damage2 = 100;
	int32 _damage3 = 100;
};

//// new operator overloding (Global)
//void* operator new(size_t size)
//{
//	cout << "new! " << size << endl;
//	void* ptr = ::malloc(size);
//	return ptr;
//}
//
//
//void operator delete(void* ptr)
//{
//	cout << "delete!" << endl;
//	::free(ptr);
//}
//
//void operator delete[](void* ptr)
//{
//	cout << "delete[]!" << endl;
//	::free(ptr);
//}
//
//void* operator new[](size_t size)
//{
//	cout << "new[]!" << size << endl;
//	void* ptr = ::malloc(size);
//	return ptr;
//}



int main()
{
	Knight* knight = (Knight*) xnew<Player>();
	knight->_hp = 100;
	xdelete(knight);
	knight->_hp = 100;
	return 0;
}