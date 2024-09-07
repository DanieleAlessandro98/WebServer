#include "network_io.h"
#include <stdio.h>
#include "buffer.h"
#include <string.h>

ERecvResult recv_all(SOCKET sock_fd, char **buf, size_t *len, size_t *buf_size)
{
    size_t remaining_space = *buf_size - *len - 1;

    if (remaining_space == 0)
    {
        if (!adjust_recv_buffer(buf, buf_size, *len))
            return RECV_BUFFER_OVERFLOW;

        remaining_space = *buf_size - *len - 1;
    }

    size_t recv_size = (remaining_space > MAX_RECV_SEGMENT_SIZE) ? MAX_RECV_SEGMENT_SIZE : remaining_space;

    int r = recv(sock_fd,
                 *buf + *len,
                 (int)recv_size, 0);

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

ESendResult send_all(SOCKET sock_fd, const char *data, const size_t len, size_t *total_sent)
{
    size_t bytes_left = len - *total_sent;

    size_t send_size = (bytes_left > MAX_SEND_SEGMENT_SIZE) ? MAX_SEND_SEGMENT_SIZE : bytes_left;

    int s = send(sock_fd,
                 data + *total_sent,
                 (int)send_size, 0);

    if (s == SOCKET_ERROR)
        return SEND_ERROR;

    *total_sent += s;

    if (*total_sent < len)
        return SEND_INCOMPLETE;

    return SEND_COMPLETE;
}
