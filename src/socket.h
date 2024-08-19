#ifndef SOCKET_H
#define SOCKET_H

#include "win_definition.h"

int create_socket(SOCKET *_socket);
int close_socket(SOCKET *_socket);

#endif // SOCKET_H