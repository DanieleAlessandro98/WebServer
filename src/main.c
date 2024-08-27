#include "win_definition.h"
#include <stdio.h>
#include "winsock_utils.h"
#include "socket.h"
#include "address.h"
#include "network_io.h"
#include "fdwatch.h"

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

    LPFDWATCH main_fdw = fdwatch_new(4096);
    fdwatch_add_fd(main_fdw, server_socket, NULL, FDW_READ);

    while (1)
    {
        int num_events, event_idx;
        if ((num_events = fdwatch(main_fdw, 0)) < 0)
            break;

        CLIENT_DATA_POINTER client_data;

        for (event_idx = 0; event_idx < num_events; ++event_idx)
        {
            client_data = (CLIENT_DATA_POINTER)fdwatch_get_client_data(main_fdw, event_idx);

            if (!client_data)
            {
                if (FDW_READ == fdwatch_check_event(main_fdw, server_socket, event_idx))
                {
                    SOCKET client_socket;
                    int accept_result = accept_socket(&server_socket, &client_socket);
                    if (accept_result == WSAEWOULDBLOCK)
                        continue;

                    if (accept_result != 0)
                    {
                        fprintf(stderr, "Failed to accept socket: %d.\n", WSAGetLastError());
                        continue;
                    }

                    fdwatch_clear_event(main_fdw, server_socket, event_idx);
                    fdwatch_add_fd(main_fdw, client_socket, client_data_new(client_socket), FDW_READ);
                }

                continue;
            }

            int iRet = fdwatch_check_event(main_fdw, client_data->socket, event_idx);

            switch (iRet)
            {
            case FDW_READ:
            {
                printf("Trying to recv data..\n");

                ERecvResult r = recv_all(&client_data->socket, client_data->recvbuf, &client_data->recvlen);
                switch (r)
                {
                case RECV_BUFFER_OVERFLOW:
                {
                    fprintf(stderr, "Max http size reached.\n");
                    close_socket(&client_data->socket);
                    fdwatch_del_fd(main_fdw, client_data->socket);
                    client_data_delete(client_data);
                    continue;
                }
                break;

                case RECV_ERROR:
                {
                    fprintf(stderr, "Failed to recv data: %d.\n", WSAGetLastError());
                    close_socket(&client_data->socket);
                    fdwatch_del_fd(main_fdw, client_data->socket);
                    client_data_delete(client_data);
                    continue;
                }
                break;

                case RECV_CLOSED:
                {
                    fprintf(stderr, "Client close connection.\n");
                    close_socket(&client_data->socket);
                    fdwatch_del_fd(main_fdw, client_data->socket);
                    client_data_delete(client_data);
                    continue;
                }
                break;

                case RECV_INCOMPLETE:
                {
                    continue;
                }
                break;

                case RECV_COMPLETE:
                {
                    printf("Data received:\n%s\n", client_data->recvbuf);
                    fdwatch_add_fd(main_fdw, client_data->socket, client_data, FDW_WRITE);
                }
                break;
                }
            }
            break;

            case FDW_WRITE:
            {
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

                ESendResult s = send_all(&client_data->socket, response, response_length, &client_data->sendlen);

                switch (s)
                {
                case SEND_BUFFER_OVERFLOW:
                {
                    fprintf(stderr, "Max http size reached.\n");
                    close_socket(&client_data->socket);
                    fdwatch_del_fd(main_fdw, client_data->socket);
                    client_data_delete(client_data);
                    continue;
                }
                break;

                case SEND_ERROR:
                {
                    fprintf(stderr, "Failed to send data: %d.\n", WSAGetLastError());
                    close_socket(&client_data->socket);
                    fdwatch_del_fd(main_fdw, client_data->socket);
                    client_data_delete(client_data);
                    continue;
                }
                break;

                case SEND_INCOMPLETE:
                {
                    continue;
                }
                break;

                case SEND_COMPLETE:
                {
                    printf("Sending data completed.\n");

                    if (shutdown(client_data->socket, SD_SEND) == SOCKET_ERROR)
                    {
                        fprintf(stderr, "Shutdown error: %d\n", WSAGetLastError());
                        close_socket(&client_data->socket);
                        fdwatch_del_fd(main_fdw, client_data->socket);
                        client_data_delete(client_data);
                        continue;
                    }

                    close_socket(&client_data->socket);
                    fdwatch_del_fd(main_fdw, client_data->socket);
                    client_data_delete(client_data);
                    continue;
                }
                break;
                }
            }
            break;

            case FDW_EOF:
                close_socket(&client_data->socket);
                fdwatch_del_fd(main_fdw, client_data->socket);
                client_data_delete(client_data);
                break;

            default:
                close_socket(&client_data->socket);
                fdwatch_del_fd(main_fdw, client_data->socket);
                client_data_delete(client_data);
                fprintf(stderr, "fdwatch_check_event returned unknown %d, socket = %d\n", iRet, client_data->socket);
                break;
            }
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
