#include "win_definition.h"
#include <stdio.h>
#include "winsock_utils.h"
#include "socket.h"
#include "address.h"
#include "network_io.h"
#include "fdwatch.h"

int cleanup_and_exit(SOCKET *server_socket, LPFDWATCH *main_fdw, int exit_code);
void close_client_session(LPFDWATCH main_fdw, CLIENT_DATA_POINTER client_data);

int main()
{
    if (!initialize_winsock())
    {
        fprintf(stderr, "Failed to initialize Winsock: %d.\n", WSAGetLastError());
        return EXIT_FAILURE;
    }

    SOCKET server_socket;
    if (!create_socket(&server_socket))
    {
        fprintf(stderr, "Failed to create socket: %d.\n", WSAGetLastError());
        cleanup_and_exit(NULL, NULL, EXIT_FAILURE);
    }

    NetAddress address;
    EAddressResult address_result = set_address(&address, "localhost", 8080);
    if (address_result != ADDRESS_SUCCESS)
    {
        switch (address_result)
        {
        case ADDRESS_ERR_INVALID_IP:
            fprintf(stderr, "Failed to set IP address: Invalid IP format.\n");
            break;

        case ADDRESS_ERR_DNS_RESOLUTION_FAILED:
            fprintf(stderr, "Failed to resolve DNS: Unable to resolve the hostname.\n");
            break;
        }

        cleanup_and_exit(&server_socket, NULL, EXIT_FAILURE);
    }

    if (!bind_socket(&server_socket, &address))
    {
        fprintf(stderr, "Failed to bind socket: %d.\n", WSAGetLastError());
        cleanup_and_exit(&server_socket, NULL, EXIT_FAILURE);
    }

    if (!listen_socket(&server_socket))
    {
        fprintf(stderr, "Failed to listen socket: %d.\n", WSAGetLastError());
        cleanup_and_exit(&server_socket, NULL, EXIT_FAILURE);
    }

    printf("Waiting connections...\n");

    LPFDWATCH main_fdw = fdwatch_new(4096);
    if (!main_fdw)
    {
        fprintf(stderr, "Failed to create fdwatch.\n");
        cleanup_and_exit(&server_socket, NULL, EXIT_FAILURE);
    }

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

                    if (!accept_result)
                    {
                        fprintf(stderr, "Failed to accept socket: %d.\n", WSAGetLastError());
                        continue;
                    }

                    fdwatch_clear_event(main_fdw, server_socket, event_idx);
                    fdwatch_add_fd(main_fdw, client_socket, client_data_new(client_socket), FDW_READ);
                }

                continue;
            }

            int event = fdwatch_check_event(main_fdw, client_data->socket, event_idx);

            switch (event)
            {
            case FDW_READ:
            {
                printf("Trying to recv data..\n");

                ERecvResult r = recv_all(&client_data->socket, client_data->recvbuf, &client_data->recvlen);
                switch (r)
                {
                case RECV_BUFFER_OVERFLOW:
                    fprintf(stderr, "Max http size reached.\n");
                    close_client_session(main_fdw, client_data);
                    break;

                case RECV_ERROR:
                    fprintf(stderr, "Failed to recv data: %d.\n", WSAGetLastError());
                    close_client_session(main_fdw, client_data);
                    break;

                case RECV_CLOSED:
                    fprintf(stderr, "Client close connection.\n");
                    close_client_session(main_fdw, client_data);
                    break;

                case RECV_INCOMPLETE:
                    break;

                case RECV_COMPLETE:
                    printf("Data received:\n%s\n", client_data->recvbuf);
                    fdwatch_add_fd(main_fdw, client_data->socket, client_data, FDW_WRITE);
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
                    fprintf(stderr, "Max http size reached.\n");
                    close_client_session(main_fdw, client_data);
                    break;

                case SEND_ERROR:
                    fprintf(stderr, "Failed to send data: %d.\n", WSAGetLastError());
                    close_client_session(main_fdw, client_data);
                    break;

                case SEND_INCOMPLETE:
                    break;

                case SEND_COMPLETE:
                    printf("Sending data completed.\n");

                    if (shutdown(client_data->socket, SD_SEND) == SOCKET_ERROR)
                        fprintf(stderr, "Shutdown error: %d\n", WSAGetLastError());

                    close_client_session(main_fdw, client_data);
                    break;
                }
            }
            break;

            case FDW_EOF:
            default:
                fprintf(stderr, "fdwatch_check_event returned unknown %d, socket = %d\n", event, client_data->socket);
                close_client_session(main_fdw, client_data);
                break;
            }
        }
    }

    cleanup_and_exit(&server_socket, &main_fdw, EXIT_SUCCESS);
    return 0;
}

int cleanup_and_exit(SOCKET *server_socket, LPFDWATCH *main_fdw, int exit_code)
{
    if (server_socket && *server_socket != INVALID_SOCKET)
        close_socket(server_socket);

    if (main_fdw && *main_fdw)
        fdwatch_delete(*main_fdw);

    cleanup_winsock();
    system("pause");
    exit(exit_code);
}

void close_client_session(LPFDWATCH main_fdw, CLIENT_DATA_POINTER client_data)
{
    close_socket(&client_data->socket);
    fdwatch_del_fd(main_fdw, client_data->socket);
    client_data_delete(client_data);
}
