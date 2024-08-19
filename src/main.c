#include "win_definition.h"
#include <stdio.h>
#include "winsock_utils.h"
#include "socket.h"
#include "address.h"

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
