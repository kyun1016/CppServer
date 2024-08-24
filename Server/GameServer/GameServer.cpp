#include "pch.h"
#include <iostream>
#include "CorePch.h"

int main()
{
    std::unique_ptr<ServerCore::CorePch> server;
    
    server = std::make_unique< ServerCore::CorePch>();

    server->HelloWorld();

}