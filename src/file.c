#include "file.h"
#include "win_definition.h"
#include <stdio.h>
#include "http.h"

bool is_file_exists(const char *full_path)
{
    FILE *fp = fopen(full_path, "rb");
    if (!fp)
        return false;

    fclose(fp);
    return true;
}

size_t get_file_lenght(const char *full_path)
{
    FILE *fp = fopen(full_path, "rb");
    if (!fp)
        return 0;

    fseek(fp, 0L, SEEK_END);
    size_t content_length = ftell(fp);
    rewind(fp);

    fclose(fp);
    return content_length;
}

size_t read_file_into_buffer(const char *full_path, char *buffer, size_t buffer_size)
{
    FILE *fp = fopen(full_path, "rb");
    if (!fp)
        return 0;

    size_t total_bytes_read = 0;

    while (total_bytes_read < buffer_size)
    {
        char *write_position = buffer + total_bytes_read;
        size_t remaining_size = buffer_size - total_bytes_read;

        size_t bytes_read = fread(write_position, 1, remaining_size, fp);
        if (bytes_read == 0)
            break;

        total_bytes_read += bytes_read;
    }

    fclose(fp);
    return total_bytes_read;
}
