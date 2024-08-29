#ifndef FILE_H
#define FILE_H

#include <stdbool.h>
#include <stddef.h>

bool is_file_exists(const char *path);
size_t get_file_lenght(const char *full_path);
size_t read_file_into_buffer(const char *full_path, char *buffer, size_t buffer_size);

#endif
