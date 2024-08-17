#include <stdio.h>
#include "winsock_utils.h"
#include "socket.h"

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
