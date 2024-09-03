#ifndef HTTP_H
#define HTTP_H

#include "fdwatch.h"

typedef struct responsestatus RESPONSE_STATUS;
typedef struct httpcontentdata HTTP_CONTENT_DATA;

typedef enum
{
    HTTP_OK,
    HTTP_BAD_REQUEST,
    HTTP_NOT_FOUND,
    HTTP_INTERNAL_ERROR,
} HttpStatus;

struct responsestatus
{
    int status_code;
    const char *status_text;
};

struct httpcontentdata
{
    const char *content_type;
    size_t content_length;
    const char *content;
};

void handle_http_request(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data);
HttpStatus process_http_request(const char *request, char *full_path);
HttpStatus process_http_path(const char *request, char *full_path);
HttpStatus process_http_file(const char *path, char *full_path);

void create_http_header(char *buffer, size_t buffer_size, RESPONSE_STATUS response_status, HTTP_CONTENT_DATA content_data);
void send_file_response(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data, RESPONSE_STATUS response_status, char *full_path);
void send_internal_server_error(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data);
void send_response(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data, RESPONSE_STATUS response_status, HTTP_CONTENT_DATA content_data);

#endif
