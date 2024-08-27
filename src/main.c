#include "win_definition.h"
#include <stdio.h>
#include "winsock_utils.h"
#include "socket.h"
#include "address.h"
#include "network_io.h"

int main()
{
    int winsock_result = initialize_winsock();
    if (winsock_result != 0)
    {
        fprintf(stderr, "Failed to initialize Winsock: %d.\n", winsock_result);
        return 1;
    }

    SOCKET server_socket;
    if (create_socket(&server_socket) != 0)
    {
        fprintf(stderr, "Failed to create socket: %d.\n", WSAGetLastError());
        cleanup_winsock();
        return 1;
    }

    NetAddress address;
    AddressResult address_result = set_address(&address, "localhost", 8080);
    switch (address_result)
    {
    case ADDRESS_ERR_INVALID_IP:
        fprintf(stderr, "Failed to set IP address: Invalid IP format.\n");
        close_socket(&server_socket);
        cleanup_winsock();
        return 1;

    case ADDRESS_ERR_DNS_RESOLUTION_FAILED:
        fprintf(stderr, "Failed to resolve DNS: Unable to resolve the hostname.\n");
        close_socket(&server_socket);
        cleanup_winsock();
        return 1;
    }

    if (bind_socket(&server_socket, &address) != 0)
    {
        fprintf(stderr, "Failed to bind socket: %d.\n", WSAGetLastError());
        close_socket(&server_socket);
        cleanup_winsock();
        return 1;
    }

    if (listen_socket(&server_socket) != 0)
    {
        fprintf(stderr, "Failed to listen socket: %d.\n", WSAGetLastError());
        close_socket(&server_socket);
        cleanup_winsock();
        return 1;
    }

    printf("Waiting connections...\n");

    while (1)
    {
        SOCKET client_socket;
        int accept_result = accept_socket(&server_socket, &client_socket);
        if (accept_result == WSAEWOULDBLOCK)
        {
            Sleep(100);
            continue;
        }
        else if (accept_result != 0)
        {
            fprintf(stderr, "Failed to accept socket: %d.\n", accept_result);
            break;
        }

        char recvbuf[MAX_HTTP_REQUEST_SIZE];
        int recvbuflen = MAX_HTTP_REQUEST_SIZE;
        int r = recv_all(&client_socket, recvbuf, recvbuflen);
        if (r == SOCKET_ERROR)
        {
            fprintf(stderr, "Failed to recv data: %d.\n", WSAGetLastError());
            close_socket(&client_socket);
            break;
        }

        printf("%s\n", recvbuf);

        printf("Trying to send data..\n");

        const char *message = "test send text";
        int message_length = strlen(message);

        char response[MAX_HTTP_RESPONSE_SIZE];
        int response_length = snprintf(response,
                                       MAX_HTTP_RESPONSE_SIZE,
                                       "HTTP/1.1 200 OK\r\n"
                                       "Content-Type: text/plain\r\n"
                                       "Content-Length: %d\r\n"
                                       "Connection: close\r\n"
                                       "\r\n"
                                       "%s",
                                       message_length, message);

        int s = send_all(&client_socket, response, response_length);
        if (s == SOCKET_ERROR)
        {
            fprintf(stderr, "Failed to send data: %d.\n", WSAGetLastError());
            close_socket(&client_socket);
            break;
        }

        if (shutdown(client_socket, SD_SEND) == SOCKET_ERROR)
        {
            fprintf(stderr, "Shutdown error: %d\n", WSAGetLastError());
            close_socket(&client_socket);
            break;
        }

        if (close_socket(&client_socket) != 0)
        {
            fprintf(stderr, "Failed to close client socket: %d.\n", WSAGetLastError());
            break;
        }
    }

    if (close_socket(&server_socket) != 0)
    {
        fprintf(stderr, "Failed to close socket: %d.\n", WSAGetLastError());
        cleanup_winsock();
        return 1;
    }

    printf("test\n");

    cleanup_winsock();
    return 0;
}
