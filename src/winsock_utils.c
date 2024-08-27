#include <stdio.h>
#include <winsock2.h>
#include "winsock_utils.h"

bool initialize_winsock()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return false;

    return true;
}

void cleanup_winsock()
{
    WSACleanup();
}
