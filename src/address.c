#include "address.h"

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

int inet_pton(int af, const char *src, void *dst)
{
    struct sockaddr_storage ss;
    int size = sizeof(ss);
    char src_copy[INET6_ADDRSTRLEN + 1];

    ZeroMemory(&ss, sizeof(ss));
    /* stupid non-const API */
    strncpy(src_copy, src, INET6_ADDRSTRLEN + 1);
    src_copy[INET6_ADDRSTRLEN] = 0;

    if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0)
    {
        switch (af)
        {
        case AF_INET:
            *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
            return 1;
        case AF_INET6:
            *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
            return 1;
        }
    }
    return 0;
}

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    struct sockaddr_storage ss;
    unsigned long s = size;

    ZeroMemory(&ss, sizeof(ss));
    ss.ss_family = af;

    switch (af)
    {
    case AF_INET:
        ((struct sockaddr_in *)&ss)->sin_addr = *(struct in_addr *)src;
        break;
    case AF_INET6:
        ((struct sockaddr_in6 *)&ss)->sin6_addr = *(struct in6_addr *)src;
        break;
    default:
        return NULL;
    }
    /* cannot direclty use &size because of strict aliasing rules */
    return (WSAAddressToString((struct sockaddr *)&ss, sizeof(ss), NULL, dst, &s) == 0) ? dst : NULL;
}
