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
	cout << "vector" << endl;
	Vector<Knight> v(10);
	cout << "list" << endl;
	List<Knight> l(10);

	cout << "map" << endl;
	Map<int32, Knight> m;
	m.insert(std::make_pair<int32, Knight>(0, Knight()));
	cout << m[0]._hp << endl;

	return 0;
}