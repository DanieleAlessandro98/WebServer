#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include "definitions.h"
#include "fdwatch.h"

void process_new_connection(SOCKET server_socket, LPFDWATCH main_fdw, unsigned int event_idx);
void process_client_read(LPFDWATCH main_fdw, CLIENT_DATA_POINTER client_data);
void process_client_write(LPFDWATCH main_fdw, CLIENT_DATA_POINTER client_data);
void close_client_session(LPFDWATCH main_fdw, SOCKET client_socket, CLIENT_DATA_POINTER client_data);

#endif
