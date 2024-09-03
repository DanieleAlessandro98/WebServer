#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

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
