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
    void SetTarget(WraightRef target)   // 하지만, 해당 방법은 복사하는데 비용이 들기 때문에, & ref 형태로 옮겨주는 방식도 하나의 방법이다.
    {
        _target = target;
    }

    bool Update()
    {
        if (_target == nullptr)
            return false;

        int posX = _target->_posX;
        int posY = _target->_posY;

        // TODO: 쫓아간다
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