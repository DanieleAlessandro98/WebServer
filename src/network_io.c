#include "network_io.h"
#include <stdio.h>

int recv_all(SOCKET *sock_fd, char *buf, int total_len)
{
    int current_len = 0;
    int bytes_left = total_len;

    while (current_len < total_len)
    {
        int r = recv(*sock_fd, buf + current_len, bytes_left, 0);

        if (r == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (r == 0)
            break;

        current_len += r;
        bytes_left -= r;

        buf[current_len] = 0;

        char *q = strstr(buf, "\r\n\r\n");
        if (q)
            break;
    }

    return current_len;
}
