#include "address.h"
#include <Ws2tcpip.h>

AddressResult set_address(NetAddress *address, const char *c_szAddr, int port)
{
    if (is_address_ip(c_szAddr))
    {
        if (!set_address_ip(address, c_szAddr))
            return ADDRESS_ERR_INVALID_IP;
    }
    else
    {
        if (!set_address_dsn(address, c_szAddr))
            return ADDRESS_ERR_DNS_RESOLUTION_FAILED;
    }

    set_address_port(address, port);
    return ADDRESS_SUCCESS;
}

bool set_address_ip(NetAddress *address, const char *c_szIP)
{
    struct in_addr addr;
    if (inet_pton(AF_INET, c_szIP, &addr) != 1 || addr.S_un.S_addr == INADDR_NONE)
        return false;

    address->sockAddrIn.sin_addr.s_addr = addr.S_un.S_addr;
    return true;
}

bool set_address_dsn(NetAddress *address, const char *c_szDNS)
{
    struct addrinfo hints;
    struct addrinfo *result = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int ret = getaddrinfo(c_szDNS, NULL, &hints, &result);
    if (ret != 0 || result == NULL)
        return false;

    struct sockaddr_in *addr = (struct sockaddr_in *)result->ai_addr;
    address->sockAddrIn.sin_addr = addr->sin_addr;

    freeaddrinfo(result);
    return true;
}

void set_address_port(NetAddress *address, int port)
{
    address->sockAddrIn.sin_port = htons(port);
}

bool is_address_ip(const char *c_szAddr)
{
    struct in_addr addr;
    return inet_pton(AF_INET, c_szAddr, &addr) == 1;
}
