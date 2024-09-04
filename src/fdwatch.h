#ifndef FDWATCH_H
#define FDWATCH_H

#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"

#define CREATE(result, type, number)                                       \
    do                                                                     \
    {                                                                      \
        if (!((result) = (type *)calloc((number), sizeof(type))))          \
        {                                                                  \
            char error_buffer[256];                                        \
            strerror_s(error_buffer, sizeof(error_buffer), errno);         \
            fprintf(stderr, "calloc failed [%d] %s", errno, error_buffer); \
            abort();                                                       \
        }                                                                  \
    } while (0)

typedef struct fdwatch FDWATCH;
typedef struct fdwatch *LPFDWATCH;

typedef struct clientdata CLIENT_DATA;
typedef struct clientdata *CLIENT_DATA_POINTER;

enum EFdwatch
{
    FDW_NONE = 0,
    FDW_READ = 1,
    FDW_WRITE = 2,
    FDW_EOF = 8,
};

struct fdwatch
{
    // Used with "select" to determine which file descriptors are ready for reading
    fd_set rfd_set;

    // Used with "select" to determine which file descriptors are ready for writing
    fd_set wfd_set;

    // Contains the file descriptors actually monitored for reading and/or writing. Each element represents a file descriptor to monitor
    SOCKET *select_fds;

    // After a call to select, it is filled with the indices of the file descriptors that have events ready (reading or writing)
    int *select_rfdidx;

    // Indicates how many file descriptors are present in "select_fds" and are currently being monitored
    int nselect_fds;

    // Copy of rfd_set that is passed to the select call. It is used to avoid modifying the original set during the use of select
    fd_set working_rfd_set;

    // Copy of wfd_set that is passed to the select call. It is used to avoid modifying the original set during the use of select
    fd_set working_wfd_set;

    // Indicates the maximum size of "select_fds", "select_rfdidx", and "fd_data"
    int nfiles;

    // Stores additional data associated with each monitored file descriptor
    void **fd_data;

    // Flags that indicate the events being monitored for each file descriptor (reading, writing, or both)
    int *fd_rw;
};

// Structure that defines the data for each connected client
struct clientdata
{
    SOCKET socket;

    char *recvbuf;
    size_t recvbufsize;
    size_t recvlen;

    char *sendbuf;
    size_t sendbufsize;
    size_t totalsendlen;
    size_t sendlen;
};

// Initializes the watcher
extern LPFDWATCH fdwatch_new(int nfiles);

// Initializes client data
extern CLIENT_DATA_POINTER client_data_new(SOCKET client_socket);

// Destroys the watcher
extern void fdwatch_delete(LPFDWATCH fdw);

// Destroys client data
extern void client_data_delete(CLIENT_DATA_POINTER client_data);

// Checks if a file descriptor is ready for reading or writing
extern int fdwatch_check_fd(LPFDWATCH fdw, SOCKET fd);

// Verifies if a certain event has occurred for a specific file descriptor
extern int fdwatch_check_event(LPFDWATCH fdw, SOCKET fd, unsigned int event_idx);

// Clears a specific event for a file descriptor
extern void fdwatch_clear_event(LPFDWATCH fdw, SOCKET fd, unsigned int event_idx);

// Adds a file descriptor to be monitored, specifying the types of events (reading/writing)
extern void fdwatch_add_fd(LPFDWATCH fdw, SOCKET fd, void *client_data, int rw);

// Monitors the file descriptors to check which ones are ready for reading or writing, within the specified timeout
extern int fdwatch(LPFDWATCH fdw, struct timeval *timeout);

// Retrieves the client data associated with a ready file descriptor
extern void *fdwatch_get_client_data(LPFDWATCH fdw, unsigned int event_idx);

// Removes a file descriptor from the list of monitored file descriptors
extern void fdwatch_del_fd(LPFDWATCH fdw, SOCKET fd);

#endif
