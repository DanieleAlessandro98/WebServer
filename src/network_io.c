#include "network_io.h"
#include <stdio.h>

ERecvResult recv_all(SOCKET *sock_fd, char *buf, int *len)
{
    if (*len >= MAX_HTTP_REQUEST_SIZE)
        return RECV_BUFFER_OVERFLOW;

    int r = recv(*sock_fd,
                 buf + *len,
                 MAX_HTTP_REQUEST_SIZE - *len, 0);

    if (r == SOCKET_ERROR)
        return RECV_ERROR;

    if (r == 0)
        return RECV_CLOSED;

    *len += r;
    buf[*len] = 0;

    char *q = strstr(buf, "\r\n\r\n");
    if (!q)
        return RECV_INCOMPLETE;

    return RECV_COMPLETE;
}

ESendResult send_all(SOCKET *sock_fd, const char *data, const int len, int *total_sent)
{
    if (len >= MAX_HTTP_REQUEST_SIZE)
        return RECV_BUFFER_OVERFLOW;

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
