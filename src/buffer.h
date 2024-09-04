#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdbool.h>

bool adjust_recv_buffer(char **buf, size_t *buf_size, size_t data_length);
bool adjust_send_buffer(char **buf, size_t *buf_size, size_t required_size);

#endif
