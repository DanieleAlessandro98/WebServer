#include "http.h"
#include "win_definition.h"

void handle_http_request(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data)
{
    HttpStatus status = process_http_request(client_data->recvbuf);
    switch (status)
    {
    case HTTP_BAD_REQUEST:
        send_400(fdw, client_data);
        break;

    case HTTP_NOT_FOUND:
        send_404(fdw, client_data);
        break;

    case HTTP_OK:
        send_200(fdw, client_data);
        break;
    }
}

HttpStatus process_http_request(const char *request)
{
    if (request == NULL || strlen(request) > MAX_HTTP_RESPONSE_SIZE)
        return HTTP_BAD_REQUEST;

    if (strncmp(request, "GET ", 4) != 0)
        return HTTP_BAD_REQUEST;

    const char *http_version = strstr(request, "HTTP/");
    if (http_version == NULL || (strncmp(http_version, "HTTP/1.1", 8) != 0 && strncmp(http_version, "HTTP/1.0", 8) != 0))
        return HTTP_BAD_REQUEST;

    if (strstr(request, "Host:") == NULL)
        return HTTP_BAD_REQUEST;

    return process_http_path(request);
}

HttpStatus process_http_path(const char *request)
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
    
    return HTTP_OK;
}

void send_400(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data)
{
    const char *message = "<html><body><h1>400 Bad Request</h1></body></html>";
    int message_length = strlen(message);

    char response[MAX_HTTP_RESPONSE_SIZE];
    int response_length = snprintf(response,
                                   MAX_HTTP_RESPONSE_SIZE,
                                   "HTTP/1.1 400 Bad Request\r\n"
                                   "Content-Type: text/html\r\n"
                                   "Content-Length: %d\r\n"
                                   "Connection: close\r\n"
                                   "\r\n"
                                   "%s",
                                   message_length, message);

    snprintf(client_data->sendbuf, sizeof(client_data->sendbuf), "%s", response);
    client_data->totalsendlen = response_length;

    fdwatch_add_fd(fdw, client_data->socket, client_data, FDW_WRITE);
}

void send_404(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data)
{
    const char *message = "<html><body><h1>404 Not Found</h1></body></html>";
    int message_length = strlen(message);

    char response[MAX_HTTP_RESPONSE_SIZE];
    int response_length = snprintf(response,
                                   MAX_HTTP_RESPONSE_SIZE,
                                   "HTTP/1.1 404 Not Found\r\n"
                                   "Content-Type: text/html\r\n"
                                   "Content-Length: %d\r\n"
                                   "Connection: close\r\n"
                                   "\r\n"
                                   "%s",
                                   message_length, message);

    snprintf(client_data->sendbuf, sizeof(client_data->sendbuf), "%s", response);
    client_data->totalsendlen = response_length;

    fdwatch_add_fd(fdw, client_data->socket, client_data, FDW_WRITE);
}

void send_200(LPFDWATCH fdw, CLIENT_DATA_POINTER client_data)
{
    const char *message = "<html><body><h1>200 OK</h1></body></html>";
    int message_length = strlen(message);

    char response[MAX_HTTP_RESPONSE_SIZE];
    int response_length = snprintf(response,
                                   MAX_HTTP_RESPONSE_SIZE,
                                   "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: text/html\r\n"
                                   "Content-Length: %d\r\n"
                                   "Connection: close\r\n"
                                   "\r\n"
                                   "%s",
                                   message_length, message);

    snprintf(client_data->sendbuf, sizeof(client_data->sendbuf), "%s", response);
    client_data->totalsendlen = response_length;

    fdwatch_add_fd(fdw, client_data->socket, client_data, FDW_WRITE);
}
