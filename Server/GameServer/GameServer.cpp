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
    // 1) �̹� ������� Ŭ���� ������� ��� �Ұ�
    // 2) ��ȯ (Cycle) ����

    
    // RefCountBlock (useCount(shared), weakCount);
    shared_ptr<Knight> spr = make_shared<Knight>();
    weak_ptr<Knight> wpr = spr;

    bool expire = wpr.expired();
    shared_ptr<Knight> spr2 = wpr.lock();
    
    return 0;
}