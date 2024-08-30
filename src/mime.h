#ifndef MIME_H
#define MIME_H

#include <stdbool.h>

typedef struct
{
    const char *extension;
    const char *content_type;
} ContentType;

const char *get_content_type(const char *full_path);
bool is_content_type_allowed(const char *path);

#endif
