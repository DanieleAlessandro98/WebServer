#ifndef NETWORK_IO_H
#define NETWORK_IO_H

#include "definitions.h"

typedef enum
{
    RECV_BUFFER_OVERFLOW = -2,
    RECV_ERROR = -1,
    RECV_CLOSED = 0,
    RECV_INCOMPLETE = 1,
    RECV_COMPLETE = 2
} ERecvResult;

typedef enum
{
    SEND_BUFFER_OVERFLOW = -2,
    SEND_ERROR = -1,
    SEND_INCOMPLETE = 1,
    SEND_COMPLETE = 2
} ESendResult;

ERecvResult recv_all(SOCKET sock_fd, char **buf, size_t *len, size_t *buf_size);
ESendResult send_all(SOCKET sock_fd, const char *data, const size_t len, size_t *total_sent);

#endif
