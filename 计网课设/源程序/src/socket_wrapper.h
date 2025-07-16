#pragma once

#include <stdint.h>
#include <stdbool.h>

// 平台检测宏
#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <io.h>
typedef SOCKET socket_t;
typedef int socklen_t;
#define SOCKET_INVALID INVALID_SOCKET
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
typedef int socket_t;
#define SOCKET_INVALID -1
#endif

// 统一错误码
typedef enum
{
    SOCKET_SUCCESS = 0,
    SOCKET_ERROR_INIT_FAILED,
    SOCKET_ERROR_CREATE_FAILED,
    SOCKET_ERROR_BIND_FAILED,
    SOCKET_ERROR_SETSOCKOPT_FAILED,
    SOCKET_ERROR_SEND_FAILED,
    SOCKET_ERROR_RECV_FAILED,
    SOCKET_ERROR_SET_NONBLOCK_FAILED,
    SOCKET_ERROR_POLL_FAILED,
    SOCKET_ERROR_INVALID_SOCKET
} socket_error_t;

// 事件类型
typedef enum
{
    SOCKET_EVENT_READ = 1,
    SOCKET_EVENT_WRITE = 2,
    SOCKET_EVENT_ERROR = 4
} socket_event_t;

// 统一的poll结构
typedef struct
{
    socket_t fd;
    int events;
    int revents;
} socket_poll_t;

// 核心接口函数
socket_error_t socket_system_init(void);
void socket_system_cleanup(void);
socket_t socket_create(int domain, int type, int protocol);
socket_error_t socket_bind(socket_t sock, const struct sockaddr *addr, socklen_t addrlen);
socket_error_t socket_set_reuse_addr(socket_t sock, bool enable);
socket_error_t socket_set_nonblocking(socket_t sock, bool nonblocking);
socket_error_t socket_close(socket_t sock);
int socket_send_to(socket_t sock, const void *buf, size_t len, const struct sockaddr *dest_addr, socklen_t addrlen);
int socket_recv_from(socket_t sock, void *buf, size_t len, struct sockaddr *src_addr, socklen_t *addrlen);
socket_error_t socket_poll(socket_poll_t *fds, int nfds, int timeout);
const char *socket_get_error_string(socket_error_t error);
int socket_get_last_error(void);