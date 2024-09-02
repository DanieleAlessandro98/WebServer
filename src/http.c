#include "http.h"
#include "win_definition.h"
#include "file.h"
#include "mime.h"
#include "buffer.h"

void handle_http_request(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data)
{
    char full_path[MAX_PATH_LENGTH + sizeof(PUBLIC_DIR) + 1];

    HttpStatus status = process_http_request(client_data->recvbuf, full_path);
    switch (status)
    {
    case HTTP_BAD_REQUEST:
        send_400(fdw, client_data);
        break;

    case HTTP_NOT_FOUND:
        send_404(fdw, client_data);
        break;

    case HTTP_OK:
        send_resource(fdw, client_data, full_path);
        break;
    }
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
    {
        strncpy(path, "/index.html", sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';
    }

    if (!is_content_type_allowed(path))
        return HTTP_NOT_FOUND;

    snprintf(full_path, MAX_PATH_LENGTH + sizeof(PUBLIC_DIR) + 1, "%s%s", PUBLIC_DIR, path);

    normalize_path(full_path);

    if (!is_file_exists(full_path))
        return HTTP_NOT_FOUND;

    return HTTP_OK;
}

void send_400(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data)
{
    char full_path[MAX_PATH_LENGTH + sizeof(PUBLIC_DIR) + 1];
    snprintf(full_path, MAX_PATH_LENGTH + sizeof(PUBLIC_DIR) + 1, "%s%s", PUBLIC_DIR, "/400.html");

    normalize_path(full_path);

    FILE_DATA_POINTER file_data = get_file_data(full_path);
    if (file_data == NULL)
    {
        fprintf(stderr, "cannot get file data: %s\n", full_path);
        exit(3);
    }

    const char *content_type = file_data->content_type;
    size_t content_length = file_data->size;

    int header_length = snprintf(NULL, 0,
                                 "HTTP/1.1 400 Bad Request\r\n"
                                 "Content-Type: %s\r\n"
                                 "Content-Length: %zu\r\n"
                                 "Connection: close\r\n"
                                 "\r\n",
                                 content_type, content_length);

    if (!adjust_send_buffer(&client_data->sendbuf, &client_data->sendbufsize, header_length + content_length))
    {
        free_file_data(file_data);
        return;
    }

    int response_length = snprintf(client_data->sendbuf, client_data->sendbufsize,
                                   "HTTP/1.1 400 Bad Request\r\n"
                                   "Content-Type: %s\r\n"
                                   "Content-Length: %zu\r\n"
                                   "Connection: close\r\n"
                                   "\r\n",
                                   content_type, content_length);

    memcpy(client_data->sendbuf + response_length, file_data->data, file_data->size);

    client_data->totalsendlen = response_length + content_length;

    fdwatch_add_fd(fdw, client_data->socket, client_data, FDW_WRITE);
    free_file_data(file_data);
}

void send_404(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data)
{
    char full_path[MAX_PATH_LENGTH + sizeof(PUBLIC_DIR) + 1];
    snprintf(full_path, MAX_PATH_LENGTH + sizeof(PUBLIC_DIR) + 1, "%s%s", PUBLIC_DIR, "/404.html");

    normalize_path(full_path);

    FILE_DATA_POINTER file_data = get_file_data(full_path);
    if (file_data == NULL)
    {
        fprintf(stderr, "cannot get file data: %s\n", full_path);
        exit(3);
    }

    const char *content_type = file_data->content_type;
    size_t content_length = file_data->size;

    int header_length = snprintf(NULL, 0,
                                 "HTTP/1.1 404 Not Found\r\n"
                                 "Content-Type: %s\r\n"
                                 "Content-Length: %zu\r\n"
                                 "Connection: close\r\n"
                                 "\r\n",
                                 content_type, content_length);

    if (!adjust_send_buffer(&client_data->sendbuf, &client_data->sendbufsize, header_length + content_length))
    {
        free_file_data(file_data);
        return;
    }

    int response_length = snprintf(client_data->sendbuf, client_data->sendbufsize,
                                   "HTTP/1.1 404 Not Found\r\n"
                                   "Content-Type: %s\r\n"
                                   "Content-Length: %zu\r\n"
                                   "Connection: close\r\n"
                                   "\r\n",
                                   content_type, content_length);

    memcpy(client_data->sendbuf + response_length, file_data->data, file_data->size);

    client_data->totalsendlen = response_length + content_length;

    fdwatch_add_fd(fdw, client_data->socket, client_data, FDW_WRITE);
    free_file_data(file_data);
}

void send_resource(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data, const char *full_path)
{
    FILE_DATA_POINTER file_data = get_file_data(full_path);
    if (file_data == NULL)
    {
        fprintf(stderr, "cannot get file data: %s\n", full_path);
        exit(3);
    }

    const char *content_type = file_data->content_type;
    size_t content_length = file_data->size;

    int header_length = snprintf(NULL, 0,
                                 "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: %s\r\n"
                                 "Content-Length: %zu\r\n"
                                 "Connection: close\r\n"
                                 "\r\n",
                                 content_type, content_length);

    if (!adjust_send_buffer(&client_data->sendbuf, &client_data->sendbufsize, header_length + content_length))
    {
        free_file_data(file_data);
        return;
    }

    int response_length = snprintf(client_data->sendbuf, client_data->sendbufsize,
                                   "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: %s\r\n"
                                   "Content-Length: %zu\r\n"
                                   "Connection: close\r\n"
                                   "\r\n",
                                   content_type, content_length);

    memcpy(client_data->sendbuf + response_length, file_data->data, file_data->size);

    client_data->totalsendlen = response_length + content_length;

    fdwatch_add_fd(fdw, client_data->socket, client_data, FDW_WRITE);
    free_file_data(file_data);
}
