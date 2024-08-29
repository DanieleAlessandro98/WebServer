#include "connection_manager.h"
#include "network_io.h"
#include "socket.h"

void process_new_connection(SOCKET server_socket, LPFDWATCH main_fdw, unsigned int event_idx)
{
    SOCKET client_socket;
    int accept_result = accept_socket(&server_socket, &client_socket);
    if (accept_result == WSAEWOULDBLOCK)
        return;

    if (!accept_result)
    {
        fprintf(stderr, "Failed to accept socket: %d.\n", WSAGetLastError());
        return;
    }

    fdwatch_clear_event(main_fdw, server_socket, event_idx);
    fdwatch_add_fd(main_fdw, client_socket, client_data_new(client_socket), FDW_READ);
}

void process_client_read(LPFDWATCH main_fdw, CLIENT_DATA_POINTER client_data)
{
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
        break;
    }
}

void process_client_write(LPFDWATCH main_fdw, CLIENT_DATA_POINTER client_data)
{
    ESendResult s = send_all(&client_data->socket, client_data->sendbuf, client_data->totalsendlen, &client_data->sendlen);
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

void close_client_session(LPFDWATCH main_fdw, CLIENT_DATA_POINTER client_data)
{
    close_socket(&client_data->socket);
    fdwatch_del_fd(main_fdw, client_data->socket);
    client_data_delete(client_data);
}
