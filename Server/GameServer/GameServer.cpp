#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>

void HellowThread()
{
    cout << "Hello Thread" << endl;
}
void HellowThread2(int32 num)
{
    cout << num << endl;
}

// t.get_id();                  // Thread ID
// t.hardware_concurrency();    // CPU 코어 개수
// t.detach();                  // Thread 객체에서 실제 쓰레드 분리
// t.joinable())                
// t.join();

int main()
{
    std::unique_ptr<ServerCore::CorePch> server;
    
    server = std::make_unique< ServerCore::CorePch>();

    

    std::vector<std::thread> v;

    for (int32 i = 0; i < 10; ++i)
    {
        v.push_back(std::thread(HellowThread2, i));
    }

    server->HelloWorld();

    for (int32 i = 0; i < 10; ++i)
    {
        if (v[i].joinable())
            v[i].join();
    }
    

    return 0;
}