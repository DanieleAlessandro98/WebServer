#include "mime.h"
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include "definitions.h"
#include "file.h"

static const ContentType allowed_content_type[] = {
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif", "image/gif"},
    {".ico", "image/x-icon"},
    {".json", "application/json"},
    {".pdf", "application/pdf"},
    {".svg", "image/svg+xml"},
    {".txt", "text/plain"},
    {".csv", "text/csv"},
};

void to_lowercase(char *str)
{
    for (char *p = str; *p != '\0'; p++)
        *p = tolower((unsigned char)*p);
}

static const char *find_content_type(const char *extension)
{
    if (strlen(extension) >= MAX_FILE_EXTENSION_LEN)
        return NULL;

    char extension_lower[MAX_FILE_EXTENSION_LEN];
    strncpy(extension_lower, extension, MAX_FILE_EXTENSION_LEN - 1);
    extension_lower[MAX_FILE_EXTENSION_LEN - 1] = '\0';

    to_lowercase(extension_lower);

    size_t num_types = sizeof(allowed_content_type) / sizeof(allowed_content_type[0]);
    for (size_t i = 0; i < num_types; ++i)
    {
        if (strcmp(extension_lower, allowed_content_type[i].extension) == 0)
            return allowed_content_type[i].content_type;
    }

    return NULL;
}

const char *get_content_type(const char *full_path)
{
    const char *extension = get_file_extension(full_path);
    if (!extension)
        return "application/octet-stream";

    const char *content_type = find_content_type(extension);
    if (content_type)
        return content_type;

    return "application/octet-stream";
}

bool is_content_type_allowed(const char *path)
{
    const char *extension = get_file_extension(path);
    if (!extension)
        return false;

    return find_content_type(extension) != NULL;
}
