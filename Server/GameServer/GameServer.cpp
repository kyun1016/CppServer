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

	int64 _hp = 10;
	int32 _damage = 100;
	int32 _damage2 = 100;
	int32 _damage3 = 100;
};

int main()
{
	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Vector<Knight> v(10);
					Map<int32, Knight> m;
					m[100] = Knight();

					this_thread::sleep_for(10ms);
				}
			});
	}

	GThreadManager->Join();
	return 0;
}