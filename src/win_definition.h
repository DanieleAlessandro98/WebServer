#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define MAX_HTTP_REQUEST_SIZE 2047
#define MAX_HTTP_RESPONSE_SIZE 8192
