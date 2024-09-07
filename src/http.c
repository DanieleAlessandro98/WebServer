#include "http.h"
#include "definitions.h"
#include "file.h"
#include "mime.h"
#include "buffer.h"
#include <string.h>

void handle_http_request(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data)
{
    if (!client_data)
        return;

    char full_path[MAX_PATH_LENGTH + sizeof(PUBLIC_DIR) + 1];
    RESPONSE_STATUS response_status;

    HttpStatus status = process_http_request(client_data->recvbuf, full_path);
    switch (status)
    {
    case HTTP_BAD_REQUEST:
        response_status = (RESPONSE_STATUS){400, "Bad Request"};
        strncpy(full_path, ERROR_400_PAGE, sizeof(full_path) - 1);
        full_path[sizeof(full_path) - 1] = '\0';
        break;

    case HTTP_NOT_FOUND:
        response_status = (RESPONSE_STATUS){404, "Not Found"};
        strncpy(full_path, ERROR_404_PAGE, sizeof(full_path) - 1);
        full_path[sizeof(full_path) - 1] = '\0';
        break;

    case HTTP_OK:
        response_status = (RESPONSE_STATUS){200, "OK"};
        break;

    default:
        send_internal_server_error(fdw, client_data);
        return;
    }

    send_file_response(fdw, client_data, response_status, full_path);
}

HttpStatus process_http_request(const char *request, char *full_path)
{
    if (request == NULL)
        return HTTP_BAD_REQUEST;

    if (strncmp(request, "GET ", 4) != 0)
        return HTTP_BAD_REQUEST;

    const char *http_version = strstr(request, "HTTP/");
    if (http_version == NULL || (strncmp(http_version, "HTTP/1.1", 8) != 0 && strncmp(http_version, "HTTP/1.0", 8) != 0))
        return HTTP_BAD_REQUEST;

    if (strstr(request, "Host:") == NULL)
        return HTTP_BAD_REQUEST;

    return process_http_path(request, full_path);
}

HttpStatus process_http_path(const char *request, char *full_path)
{
    const char *path_start = request + 4;
    const char *path_end = strstr(path_start, " ");
    if (path_end == NULL)
        return HTTP_BAD_REQUEST;

    size_t path_length = path_end - path_start;
    if (path_length == 0 || path_length >= MAX_PATH_LENGTH)
        return HTTP_BAD_REQUEST;

    char path[MAX_PATH_LENGTH + 1];
    strncpy(path, path_start, path_length);
    path[path_length] = '\0';

    if (strstr(path, "..") != NULL)
        return HTTP_BAD_REQUEST;

    if (strcmp(path, "/") == 0)
        snprintf(path, sizeof(path), "/%s", HOMEPAGE_FILE);

    return process_http_file(path, full_path);
}

HttpStatus process_http_file(const char *path, char *full_path)
{
    snprintf(full_path, MAX_PATH_LENGTH + sizeof(PUBLIC_DIR) + 1, "%s%s", PUBLIC_DIR, path);
    normalize_path(full_path);

    if (!is_content_type_allowed(full_path))
        return HTTP_NOT_FOUND;

    if (!is_file_exists(full_path))
        return HTTP_NOT_FOUND;

    return HTTP_OK;
}

void create_http_header(char *buffer, size_t buffer_size, RESPONSE_STATUS response_status, HTTP_CONTENT_DATA content_data)
{
    snprintf(buffer, buffer_size,
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n",
             response_status.status_code, response_status.status_text,
             content_data.content_type,
             content_data.content_length);
}

void send_file_response(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data, RESPONSE_STATUS response_status, char *full_path)
{
    if (!client_data)
        return;

    normalize_path(full_path);

    FILE_DATA_POINTER file_data = get_file_data(full_path);
    if (file_data == NULL)
    {
        send_internal_server_error(fdw, client_data);
        return;
    }

    HTTP_CONTENT_DATA content_data = {file_data->content_type, file_data->size, file_data->data};

    send_response(fdw, client_data, response_status, content_data);
    free_file_data(file_data);
}

void send_internal_server_error(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data)
{
    if (!client_data)
        return;

    const char *message = "<html><body><h1>500 Internal Server Error</h1></body></html>";

    RESPONSE_STATUS response_status = {500, "Internal Server Error"};
    HTTP_CONTENT_DATA content_data = {"text/html", strlen(message), message};

    send_response(fdw, client_data, response_status, content_data);
}

void send_response(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data, RESPONSE_STATUS response_status, HTTP_CONTENT_DATA content_data)
{
    if (!client_data)
        return;

    char header[MAX_HTTP_RESPONSE_HEADER_SIZE];
    create_http_header(header, sizeof(header), response_status, content_data);

    if (!adjust_send_buffer(&client_data->sendbuf, &client_data->sendbufsize, strlen(header) + content_data.content_length))
        return;

    snprintf(client_data->sendbuf, client_data->sendbufsize, "%s", header);
    memcpy(client_data->sendbuf + strlen(header), content_data.content, content_data.content_length);

    client_data->totalsendlen = strlen(header) + content_data.content_length;

    fdwatch_add_fd(fdw, client_data->socket, client_data, FDW_WRITE);
}
