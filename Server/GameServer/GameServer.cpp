#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include "ThreadManager.h"

bool IsPrime(int number)
{
    if (number <= 1)
        return false;
    if (number == 2 || number == 3)
        return true;

    for (int i = 2; i < number; ++i)
    {
        if (number % i == 0)
            return false;
    }
    return true;
}

// [start, end]
int CountPrime(int start, int end)
{
    int count = 0;
    for (int number = start; number <= end; ++number)
    {
        if (IsPrime(number))
            ++count;
    }

    return count;
}

int main()
{
    // 1000 = 168
    // 1'0000 = 1229
    // 100'0000 = 78498
    const int MAX_NUMBER = 100'0000;
    int coreCount = thread::hardware_concurrency();
    int jobCount = (MAX_NUMBER / coreCount) + 1;

    vector<thread> threads;
    threads.reserve(coreCount);
    
    atomic<int> primeCount = 0;
    for (int i = 0; i < coreCount; ++i)
    {
        int start = (i * jobCount) + 1;
        int end = min(((i + 1) * jobCount), MAX_NUMBER);

        threads.push_back(thread([start, end, &primeCount]
            {
                primeCount += CountPrime(start, end);
            }));
    }

    for (thread& t : threads)
        t.join();

    cout << primeCount << endl;


    GThreadManager->Join();
    return 0;
}