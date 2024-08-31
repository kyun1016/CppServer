#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include "ThreadManager.h"
#include "RefCounting.h"

using KnightRef = TSharedPtr<class Knight>;
using InventoryRef = TSharedPtr<class Inventory>;

class Knight : public RefCountable
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

    void SetTarget(KnightRef target)
    {
        _target = target;
    }

    KnightRef _target = nullptr;
    InventoryRef _inventory = nullptr;
};

class Inventory : public RefCountable
{
public:
    Inventory(KnightRef knight)
        : _knight(**knight)
    {

    }
    Knight& _knight;
};


int main()
{
    // 1) 이미 만들어진 클래스 대상으로 사용 불가
    // 2) 순환 (Cycle) 문제

    
    // RefCountBlock (useCount(shared), weakCount);
    shared_ptr<Knight> spr = make_shared<Knight>();
    weak_ptr<Knight> wpr = spr;

    bool expire = wpr.expired();
    shared_ptr<Knight> spr2 = wpr.lock();
    
    return 0;
}