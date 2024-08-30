#ifndef HTTP_H
#define HTTP_H

#include "fdwatch.h"

typedef enum
{
    HTTP_OK,
    HTTP_BAD_REQUEST,
    HTTP_NOT_FOUND,
    HTTP_INTERNAL_ERROR,
} HttpStatus;

void handle_http_request(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data);
HttpStatus process_http_request(const char *request, char *full_path);
HttpStatus process_http_path(const char *request, char *full_path);

void send_400(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data);
void send_404(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data);
void send_resource(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data, const char *full_path);

#endif
