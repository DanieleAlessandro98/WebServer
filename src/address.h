#ifndef ADDRESS_H
#define ADDRESS_H

#include "definitions.h"
#include <stdbool.h>

typedef struct
{
    struct sockaddr_in sockAddrIn;
} NetAddress;

typedef enum
{
    ADDRESS_SUCCESS = 0,
    ADDRESS_ERR_INVALID_IP = 1,
    ADDRESS_ERR_DNS_RESOLUTION_FAILED = 2
} EAddressResult;

EAddressResult set_address(NetAddress *address, const char *c_szAddr, int port);
bool set_address_ip(NetAddress *address, const char *c_szIP);
bool set_address_dsn(NetAddress *address, const char *c_szDNS);
void set_address_port(NetAddress *address, int port);
bool is_address_ip(const char *c_szAddr);
int my_inet_pton(int af, const char *src, void *dst);
const char *my_inet_ntop(int af, const void *src, char *dst, socklen_t size);

#endif
