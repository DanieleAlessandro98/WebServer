#ifndef FDWATCH_H
#define FDWATCH_H

#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "win_definition.h"

#define CREATE(result, type, number)                                          \
    do                                                                        \
    {                                                                         \
        if (!((result) = (type *)calloc((number), sizeof(type))))             \
        {                                                                     \
            fprintf(stderr, "calloc failed [%d] %s", errno, strerror(errno)); \
            abort();                                                          \
        }                                                                     \
    } while (0)

typedef struct fdwatch FDWATCH;
typedef struct fdwatch *LPFDWATCH;

enum EFdwatch
{
    FDW_NONE = 0,
    FDW_READ = 1,
    FDW_WRITE = 2,
    FDW_EOF = 8,
};

struct fdwatch
{
    fd_set rfd_set;
    fd_set wfd_set;

    SOCKET *select_fds;
    int *select_rfdidx;

    int nselect_fds;

    fd_set working_rfd_set;
    fd_set working_wfd_set;

    int nfiles;

    void **fd_data;
    int *fd_rw;
};

extern LPFDWATCH fdwatch_new(int nfiles);
extern void fdwatch_delete(LPFDWATCH fdw);
extern int fdwatch_check_fd(LPFDWATCH fdw, SOCKET fd);
extern int fdwatch_check_event(LPFDWATCH fdw, SOCKET fd, unsigned int event_idx);
extern void fdwatch_clear_event(LPFDWATCH fdw, SOCKET fd, unsigned int event_idx);
extern void fdwatch_add_fd(LPFDWATCH fdw, SOCKET fd, void *client_data, int rw);
extern int fdwatch(LPFDWATCH fdw, struct timeval *timeout);
extern void *fdwatch_get_client_data(LPFDWATCH fdw, unsigned int event_idx);
extern void fdwatch_del_fd(LPFDWATCH fdw, SOCKET fd);

#endif
