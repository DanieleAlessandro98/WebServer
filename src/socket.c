#include <stdio.h>
#include "socket.h"

int create_socket(SOCKET *sock_fd)
{
    *sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*sock_fd == INVALID_SOCKET)
        return WSAGetLastError();

    return 0;
}

int close_socket(SOCKET *sock_fd)
{
    int close_result = closesocket(*sock_fd);
    if (close_result != 0)
        return WSAGetLastError();

    return 0;
}
