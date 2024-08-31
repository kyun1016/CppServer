#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include "ThreadManager.h"
#include "RefCounting.h"

class Wraight : public RefCountable
{
public:
    int _hp = 150;
    int _posX = 0;
    int _posY = 0;
};

using WraightRef = TSharedPtr<Wraight>;

class Missile : public RefCountable
{
public:
    void SetTarget(WraightRef target)   // ������, �ش� ����� �����ϴµ� ����� ��� ������, & ref ���·� �Ű��ִ� ��ĵ� �ϳ��� ����̴�.
    {
        _target = target;
    }

    bool Update()
    {
        if (_target == nullptr)
            return false;

        int posX = _target->_posX;
        int posY = _target->_posY;

        // TODO: �Ѿư���
        if (_target->_hp == 0)
        {
            _target = nullptr;
            return false;
        }
        return true;
    }

    WraightRef _target = nullptr;
};
using MissileRef = TSharedPtr<Missile>;

int main()
{
    WraightRef wraight(new Wraight());
    MissileRef missile(new Missile());
    missile->SetTarget(wraight);

    wraight->_hp = 0;
    wraight = nullptr;

    while (true)
    {
        if (missile == nullptr)
            break;

        if (!missile->Update())
        {
            // delete missile
            missile = nullptr;
        }
    }
    
    
    return 0;
}