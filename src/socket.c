#include <stdio.h>
#include "socket.h"

bool set_socket_reuse(SOCKET sock_fd)
{
    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
    {
        fprintf(stderr, "Failed to setsockopt (reuse): %d.\n", GETSOCKETERROR());
        return false;
    }

    return true;
}

bool set_socket_nonblock(SOCKET sock_fd)
{
#ifdef _WIN32
    u_long mode = 1;

    int result = ioctlsocket(sock_fd, FIONBIO, &mode);
    if (result != NO_ERROR)
    {
        fprintf(stderr, "Failed to ioctlsocket (non-block): %d.\n", GETSOCKETERROR());
        return false;
    }
#else
    int flags;

    flags = fcntl(sock_fd, F_GETFL, 0);
    flags |= O_NONBLOCK;

    if (fcntl(sock_fd, F_SETFL, flags) < 0)
    {
        fprintf(stderr, "Failed to fcntl (non-block): %d.\n", GETSOCKETERROR());
        return false;
    }
#endif

    return true;
}

bool create_socket(SOCKET *sock_fd)
{
    *sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*sock_fd == INVALID_SOCKET)
        return false;

    if (!set_socket_reuse(*sock_fd))
        return false;

    return true;
}

bool bind_socket(SOCKET sock_fd, NetAddress *address)
{
    int bind_result = bind(sock_fd, (struct sockaddr *)&address->sockAddrIn, sizeof(address->sockAddrIn));
    if (bind_result != 0)
        return false;

    if (!set_socket_nonblock(sock_fd))
        return false;

    return true;
}

bool listen_socket(SOCKET sock_fd)
{
    int listen_result = listen(sock_fd, SOMAXCONN);
    if (listen_result != 0)
        return false;

    return true;
}

bool accept_socket(SOCKET sock_fd_server, SOCKET *sock_fd_client)
{
    *sock_fd_client = accept(sock_fd_server, NULL, NULL);
    if (*sock_fd_client == INVALID_SOCKET)
        return false;

    if (!set_socket_nonblock(*sock_fd_client))
        return false;

    return true;
}

void close_socket(SOCKET sock_fd)
{
    CLOSESOCKET(sock_fd);
}
