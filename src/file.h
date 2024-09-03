#ifndef FILE_H
#define FILE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct filedata FILE_DATA;
typedef struct filedata *FILE_DATA_POINTER;

struct filedata
{
    void *data;
    int size;
    void *content_type;
};

void normalize_path(char *str);

FILE_DATA_POINTER get_file_data(const char *full_path);
void free_file_data(FILE_DATA_POINTER file_data);

const char *get_file_extension(const char *full_path);
bool is_file_exists(const char *path);

#endif
