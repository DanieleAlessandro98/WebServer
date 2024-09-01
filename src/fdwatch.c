#define __LIBTHECORE__
#include "fdwatch.h"

int MIN(int a, int b)
{
	return a < b ? a : b;
}

LPFDWATCH fdwatch_new(int nfiles)
{
	LPFDWATCH fdw;

	// nfiles value is limited to FD_SETSIZE (64)
	CREATE(fdw, FDWATCH, 1);
	fdw->nfiles = MIN(nfiles, FD_SETSIZE);

	FD_ZERO(&fdw->rfd_set);
	FD_ZERO(&fdw->wfd_set);

	CREATE(fdw->select_fds, SOCKET, nfiles);
	CREATE(fdw->select_rfdidx, int, nfiles);

	fdw->nselect_fds = 0;

	CREATE(fdw->fd_rw, int, nfiles);
	CREATE(fdw->fd_data, void *, nfiles);

	return (fdw);
}

CLIENT_DATA_POINTER client_data_new(SOCKET client_socket)
{
	CLIENT_DATA_POINTER client_data;
	CREATE(client_data, CLIENT_DATA, 1);
	client_data->socket = client_socket;

	client_data->recvlen = 0;
	client_data->recvbufsize = DEFAULT_HTTP_REQUEST_SIZE + 1;
	CREATE(client_data->recvbuf, char, client_data->recvbufsize);

	client_data->sendlen = 0;
	client_data->sendbufsize = DEFAULT_HTTP_RESPONSE_SIZE + 1;
	CREATE(client_data->sendbuf, char, client_data->sendbufsize);
	client_data->totalsendlen = 0;

	return client_data;
}

void fdwatch_delete(LPFDWATCH fdw)
{
	free(fdw->fd_data);
	free(fdw->fd_rw);
	free(fdw->select_fds);
	free(fdw->select_rfdidx);
	free(fdw);
}

void client_data_delete(CLIENT_DATA_POINTER client_data)
{
	free(client_data->recvbuf);
	free(client_data->sendbuf);
	free(client_data);
}

static int fdwatch_get_fdidx(LPFDWATCH fdw, SOCKET fd)
{
	int i;
	for (i = 0; i < fdw->nselect_fds; ++i)
	{
		if (fdw->select_fds[i] == fd)
			return i;
	}

	return -1;
}

void fdwatch_add_fd(LPFDWATCH fdw, SOCKET fd, void *client_data, int rw)
{
	int idx = fdwatch_get_fdidx(fdw, fd);
	if (idx < 0)
	{
		if (fdw->nselect_fds >= fdw->nfiles)
			return;

		idx = fdw->nselect_fds;
		fdw->select_fds[fdw->nselect_fds++] = fd;
		fdw->fd_rw[idx] = rw;
	}
	else
		fdw->fd_rw[idx] |= rw;

	fdw->fd_data[idx] = client_data;

	if (rw & FDW_READ)
		FD_SET(fd, &fdw->rfd_set);

	if (rw & FDW_WRITE)
		FD_SET(fd, &fdw->wfd_set);
}

void fdwatch_del_fd(LPFDWATCH fdw, SOCKET fd)
{
	if (fdw->nselect_fds <= 0)
		return;

	int idx = fdwatch_get_fdidx(fdw, fd);
	if (idx < 0)
		return;

	--fdw->nselect_fds;

	fdw->select_fds[idx] = fdw->select_fds[fdw->nselect_fds];
	fdw->fd_data[idx] = fdw->fd_data[fdw->nselect_fds];
	fdw->fd_rw[idx] = fdw->fd_rw[fdw->nselect_fds];

	FD_CLR(fd, &fdw->rfd_set);
	FD_CLR(fd, &fdw->wfd_set);
}

int fdwatch(LPFDWATCH fdw, struct timeval *timeout)
{
	int r, i, event_idx;
	struct timeval tv;

	fdw->working_rfd_set = fdw->rfd_set;
	fdw->working_wfd_set = fdw->wfd_set;

	if (!timeout)
	{
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		r = select(0, &fdw->working_rfd_set, &fdw->working_wfd_set, (fd_set *)0, &tv);
	}
	else
	{
		tv = *timeout;
		r = select(0, &fdw->working_rfd_set, &fdw->working_wfd_set, (fd_set *)0, &tv);
	}

	if (r == -1)
		return -1;

	event_idx = 0;

	for (i = 0; i < fdw->nselect_fds; ++i)
	{
		if (fdwatch_check_fd(fdw, fdw->select_fds[i]))
			fdw->select_rfdidx[event_idx++] = i;
	}

	return event_idx;
}

int fdwatch_check_fd(LPFDWATCH fdw, SOCKET fd)
{
	int idx = fdwatch_get_fdidx(fdw, fd);
	if (idx < 0)
		return 0;

	int result = 0;
	if ((fdw->fd_rw[idx] & FDW_READ) && FD_ISSET(fd, &fdw->working_rfd_set))
		result |= FDW_READ;

	if ((fdw->fd_rw[idx] & FDW_WRITE) && FD_ISSET(fd, &fdw->working_wfd_set))
		result |= FDW_WRITE;

	return result;
}

void *fdwatch_get_client_data(LPFDWATCH fdw, unsigned int event_idx)
{
	int idx = fdw->select_rfdidx[event_idx];
	if (idx < 0 || fdw->nfiles <= idx)
		return NULL;

	return fdw->fd_data[idx];
}

void fdwatch_clear_event(LPFDWATCH fdw, SOCKET fd, unsigned int event_idx)
{
	int idx = fdw->select_rfdidx[event_idx];
	if (idx < 0 || fdw->nfiles <= idx)
		return;

	SOCKET rfd = fdw->select_fds[idx];
	if (fd != rfd)
		return;

	FD_CLR(fd, &fdw->working_rfd_set);
	FD_CLR(fd, &fdw->working_wfd_set);
}

int fdwatch_check_event(LPFDWATCH fdw, SOCKET fd, unsigned int event_idx)
{
	int idx = fdw->select_rfdidx[event_idx];
	if (idx < 0 || fdw->nfiles <= idx)
		return 0;

	SOCKET rfd = fdw->select_fds[idx];
	if (fd != rfd)
		return 0;

	int result = fdwatch_check_fd(fdw, fd);
	if (result & FDW_READ)
		return FDW_READ;
	else if (result & FDW_WRITE)
		return FDW_WRITE;

	return 0;
}
