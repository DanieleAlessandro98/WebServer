#include <stdio.h>
#include <winsock2.h>
#include "winsock_utils.h"

int initialize_winsock()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2,2), &wsaData);
    return result;
}

void cleanup_winsock()
{
    WSACleanup();
}
