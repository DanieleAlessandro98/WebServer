#ifndef SOCKET_H
#define SOCKET_H

#include "win_definition.h"
#include "address.h"

int create_socket(SOCKET *_socket);
int bind_socket(SOCKET *sock_fd, NetAddress *address);
int listen_socket(SOCKET *_socket);
int close_socket(SOCKET *_socket);

#endif // SOCKET_H