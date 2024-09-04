#include "buffer.h"
#include "definitions.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

static size_t calculate_new_buffer_size(size_t current_size, size_t required_size, bool is_recv)
{
    if (!is_recv)
        return required_size;

    size_t new_size = current_size * 2;
    return (new_size > required_size) ? new_size : required_size;
}

static void resize_buffer(char **buffer, size_t *buffer_size, size_t new_size)
{
    char *temp_buffer = realloc(*buffer, new_size);
    if (!temp_buffer)
    {
        char error_msg[256];
        strerror_s(error_msg, sizeof(error_msg), errno);
        fprintf(stderr, "Error: realloc failed [%d] %s\n", errno, error_msg);
        exit(3);
    }

    *buffer = temp_buffer;
    *buffer_size = new_size;
}

bool adjust_recv_buffer(char **buffer, size_t *buffer_size, size_t data_length)
{
    size_t required_size = data_length + 1;

    if (required_size >= *buffer_size)
    {
        size_t new_size = calculate_new_buffer_size(*buffer_size, required_size, true);
        if (new_size > MAX_HTTP_REQUEST_SIZE)
            return false;

        resize_buffer(buffer, buffer_size, new_size);
    }

    return true;
}

bool adjust_send_buffer(char **buffer, size_t *buffer_size, size_t required_size)
{
    if (required_size >= *buffer_size)
    {
        size_t new_size = calculate_new_buffer_size(*buffer_size, required_size, false);
        if (new_size > MAX_HTTP_RESPONSE_SIZE)
            return false;

        resize_buffer(buffer, buffer_size, new_size);
    }

    return true;
}
