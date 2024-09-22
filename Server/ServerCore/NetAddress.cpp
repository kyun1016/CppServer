#include "pch.h"
#include "NetAddress.h"

NetAddress::NetAddress(SOCKADDR_IN sockAddr)
{
}

NetAddress::NetAddress(wstring ip, uint16 port)
{
    ::memset(&_sockAddr, 0, sizeof(_sockAddr));
    _sockAddr.sin_family = AF_INET;
    _sockAddr.sin_addr = Ip2Adress(ip.c_str());
    _sockAddr.sin_port = ::htons(port);

}

wstring NetAddress::GetIpAddress()
{
    WCHAR buf[100];
    ::InetNtopW(AF_INET, &_sockAddr.sin_addr, buf, len32(buf));
    return wstring(buf);
}

IN_ADDR NetAddress::Ip2Adress(const WCHAR* ip)
{
    IN_ADDR address;
    ::InetPtonW(AF_INET, ip, &address);
    return address;
}
