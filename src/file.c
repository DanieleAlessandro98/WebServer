#include "file.h"
#include "win_definition.h"
#include <stdio.h>
#include "http.h"
#include <sys/stat.h>
#include "buffer.h"
#include "mime.h"
#include <ctype.h>

int hex_to_dec(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;

    return -1;
}

void normalize_path(char *str)
{
    if (str == NULL)
        return;

    char *src = str;
    char *dest = str;

    while (*src)
    {
        if ((*src == '%') && isxdigit(*(src + 1)) && isxdigit(*(src + 2)))
        {
            *dest = (char)(hex_to_dec(*(src + 1)) * 16 + hex_to_dec(*(src + 2)));
            src += 3;
        }
        else if (*src == '+')
        {
            *dest = ' ';
            src++;
        }
#if defined(_WIN32)
        else if (*src == '/')
        {
            *dest = '\\';
            src++;
        }
#endif
        else
        {
            *dest = *src;
            src++;
        }

        dest++;
    }

    *dest = '\0';
}

FILE_DATA_POINTER get_file_data(const char *full_path)
{
    struct stat file_info;
    if (stat(full_path, &file_info) == -1)
        return NULL;

    if (!(file_info.st_mode & S_IFREG))
        return NULL;

    FILE *fp = fopen(full_path, "rb");
    if (!fp)
        return NULL;

    long file_size = file_info.st_size;
    if (file_size < 0)
    {
        fclose(fp);
        return NULL;
    }

    unsigned char *buffer = malloc(file_size + 1);
    if (!buffer)
    {
        fclose(fp);
        return NULL;
    }
    unsigned char *buffer_offset = buffer;

    size_t total_bytes_read = 0;
    size_t bytes_remaining = file_size;

    while (bytes_remaining > 0)
    {
        size_t bytes_read = fread(buffer_offset, 1, bytes_remaining, fp);
        if (bytes_read == -1)
        {
            free(buffer);
            fclose(fp);
            return NULL;
        }

        if (bytes_read == 0)
            break;

        buffer_offset += bytes_read;
        bytes_remaining -= bytes_read;
        total_bytes_read += bytes_read;
    }

    FILE_DATA_POINTER file_data = malloc(sizeof(struct filedata));
    if (!file_data)
    {
        free(buffer);
        fclose(fp);
        return NULL;
    }

    const char *content_type_str = get_content_type(full_path);
    char *content_type = malloc(strlen(content_type_str) + 1);
    if (!content_type)
    {
        free(buffer);
        free(file_data);
        fclose(fp);
        return NULL;
    }

    strcpy(content_type, content_type_str);

    file_data->data = buffer;
    file_data->size = total_bytes_read;
    file_data->content_type = content_type;

    fclose(fp);
    return file_data;
}

void free_file_data(FILE_DATA_POINTER file_data)
{
    free(file_data->data);
    free(file_data->content_type);
    free(file_data);
}

const char *get_file_extension(const char *full_path)
{
    const char *dot = strrchr(full_path, '.');
    if (!dot)
        return NULL;

    return dot;
}

bool is_file_exists(const char *full_path)
{
    FILE *fp = fopen(full_path, "rb");
    if (!fp)
        return false;

    fclose(fp);
    return true;
}
