#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include "ThreadManager.h"

#include "PlayerManager.h"
#include "AccountManager.h"

int main()
{

    GThreadManager->Launch([=]
        {
            while (true)
            {
                cout << "Player Then Account" << endl;
                GPlayerManager.PlayerThenAccount();
                this_thread::sleep_for(100ms);
            }
        });

    GThreadManager->Launch([=]
        {
            while (true)
            {
                cout << "Account Then Player" << endl;
                GAccountManager.AccountThenPlayer();
                this_thread::sleep_for(100ms);
            }
        });

    GThreadManager->Join();
    return 0;
}