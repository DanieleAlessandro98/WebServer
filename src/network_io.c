#include "network_io.h"
#include <stdio.h>
#include "buffer.h"

ERecvResult recv_all(SOCKET *sock_fd, char **buf, int *len, int *buf_size)
{
    int remaining_space = *buf_size - *len - 1;

    if (remaining_space <= 0)
    {
        if (!adjust_recv_buffer(buf, buf_size, *len))
            return RECV_BUFFER_OVERFLOW;

        remaining_space = *buf_size - *len - 1;
    }

    int r = recv(*sock_fd,
                 *buf + *len,
                 remaining_space, 0);

    if (r == SOCKET_ERROR)
        return RECV_ERROR;

    if (r == 0)
        return RECV_CLOSED;

    *len += r;
    (*buf)[*len] = '\0';

    char *q = strstr(*buf, "\r\n\r\n");
    if (!q)
        return RECV_INCOMPLETE;

    return RECV_COMPLETE;
}

ESendResult send_all(SOCKET *sock_fd, const char *data, const int len, int *total_sent)
{
    int bytes_left = len - *total_sent;

    int s = send(*sock_fd,
                 data + *total_sent,
                 bytes_left, 0);

    if (s == SOCKET_ERROR)
        return SEND_ERROR;

    *total_sent += s;

    if (*total_sent < len)
        return SEND_INCOMPLETE;

    return SEND_COMPLETE;
}
