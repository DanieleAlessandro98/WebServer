#ifndef SOCKET_H
#define SOCKET_H

#include <stdbool.h>
#include <winsock2.h>

int create_socket(SOCKET *_socket);
int close_socket(SOCKET *_socket);

#endif SOCKET_H