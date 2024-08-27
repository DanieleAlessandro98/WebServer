#include <stdio.h>
#include "socket.h"

int create_socket(SOCKET *sock_fd)
{
    *sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*sock_fd == INVALID_SOCKET)
        return WSAGetLastError();

    u_long mode = 1;
    int result = ioctlsocket(*sock_fd, FIONBIO, &mode);
    if (result != NO_ERROR)
        return WSAGetLastError();

    return 0;
}

int bind_socket(SOCKET *sock_fd, NetAddress *address)
{
    int bind_result = bind(*sock_fd, (struct sockaddr *)&address->sockAddrIn, sizeof(address->sockAddrIn));
    if (bind_result != 0)
        return WSAGetLastError();

    return 0;
}

int listen_socket(SOCKET *sock_fd)
{
    int listen_result = listen(*sock_fd, SOMAXCONN);
    if (listen_result != 0)
        return WSAGetLastError();

    return 0;
}

int accept_socket(SOCKET *sock_fd_server, SOCKET *sock_fd_client)
{
    *sock_fd_client = accept(*sock_fd_server, NULL, NULL);
    if (*sock_fd_client == INVALID_SOCKET)
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
