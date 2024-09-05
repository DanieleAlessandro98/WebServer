#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "winsock_utils.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#endif

#ifdef _WIN32
#define INET_PTON my_inet_pton
#define GETSOCKETERROR() (WSAGetLastError())
#define CLOSESOCKET(s) closesocket(s)
#define SOCKET_WOULDBLOCK() (GETSOCKETERROR() == WSAEWOULDBLOCK)
#define SOCKET_SHUTDOWN_WRITE SD_SEND
#else
#define ZeroMemory(Destination, Length) memset((Destination), 0, (Length))
#define INET_PTON inet_pton
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define GETSOCKETERROR() (errno)
#define CLOSESOCKET(s) close(s)
#define SOCKET_WOULDBLOCK() (GETSOCKETERROR() == EWOULDBLOCK || GETSOCKETERROR() == EAGAIN)
#define SOCKET_SHUTDOWN_WRITE SHUT_WR
#endif

#define MAX_HTTP_REQUEST_SIZE 5 * 1024 * 1024  // 5MB
#define MAX_HTTP_RESPONSE_SIZE 5 * 1024 * 1024 // 5MB

#define DEFAULT_HTTP_REQUEST_SIZE 8 * 1024  // 8KB
#define DEFAULT_HTTP_RESPONSE_SIZE 8 * 1024 // 8KB

#define MAX_RECV_SEGMENT_SIZE 4 * 1024 // 4KB
#define MAX_SEND_SEGMENT_SIZE 4 * 1024 // 4KB

#define MAX_HTTP_RESPONSE_HEADER_SIZE 1024

#define MAX_PATH_LENGTH 100
#define MAX_FILE_EXTENSION_LEN 6

#define PUBLIC_DIR "public"

#define HOMEPAGE_FILE "index.html"

#define ERROR_400_PAGE "public\\400.html"
#define ERROR_404_PAGE "public\\404.html"

#endif
