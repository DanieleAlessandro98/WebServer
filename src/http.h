#ifndef HTTP_H
#define HTTP_H

#include "fdwatch.h"

typedef enum
{
    HTTP_OK,
    HTTP_BAD_REQUEST,
    HTTP_NOT_FOUND,
} HttpStatus;

void handle_http_request(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data);
HttpStatus process_http_request(const char *request);
HttpStatus process_http_path(const char *request);

void send_400(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data);
void send_404(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data);
void send_200(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data);

#endif
