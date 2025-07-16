#include "socket_wrapper.h"
#include <stdlib.h>
#include <string.h>
/**
 * 跨平台Socket封装库兼容性总结
 *
 * 本库通过预处理器指令、类型定义和接口抽象，抹平了Windows (Winsock)与POSIX (Berkeley Sockets)之间的API差异。
 * 下表清晰地总结了各关键点的兼容方法：
 *
 * +----------------+-------------------------------------+-------------------------------------+----------------------------------------------------------+
 * |      功能点    |           Windows (Winsock)         |       POSIX (Berkeley Sockets)      |                      封装库的兼容方案                    |
 * +----------------+-------------------------------------+-------------------------------------+----------------------------------------------------------+
 * |  库初始化      |   WSAStartup() / WSACleanup()       |   无需                                | socket_system_init/cleanup 在Windows下调用，在POSIX下为空函数。|
 * +----------------+-------------------------------------+-------------------------------------+----------------------------------------------------------+
 * |  Socket句柄    |   SOCKET (unsigned)                 |   int (文件描述符)                    | typedef 定义统一的 socket_t 类型。                       |
 * +----------------+-------------------------------------+-------------------------------------+----------------------------------------------------------+
 * |  无效Socket    |   INVALID_SOCKET                    |   -1                                | #define 定义统一的 SOCKET_INVALID 宏。                     |
 * +----------------+-------------------------------------+-------------------------------------+----------------------------------------------------------+
 * |  关闭Socket    |   closesocket(sock)                 |   close(fd)                         | socket_close() 内部使用 #ifdef 调用对应函数。              |
 * +----------------+-------------------------------------+-------------------------------------+----------------------------------------------------------+
 * |  设置非阻塞    |   ioctlsocket(sock, FIONBIO, ...)   |   fcntl(fd, F_SETFL, O_NONBLOCK)    | socket_set_nonblocking() 内部使用 #ifdef 实现两套逻辑。  |
 * +----------------+-------------------------------------+-------------------------------------+----------------------------------------------------------+
 * | I/O多路复用    |   select()                          |   poll() / epoll()                  | socket_poll() 内部使用 #ifdef 分别封装 select 和 poll。    |
 * +----------------+-------------------------------------+-------------------------------------+----------------------------------------------------------+
 * | 获取最后错误   |   WSAGetLastError()                 |   全局变量 errno                      | socket_get_last_error() 内部使用 #ifdef 返回对应值。     |
 * +----------------+-------------------------------------+-------------------------------------+----------------------------------------------------------+
 *
 */

/* Windows上的网络编程库（Winsock）要求每个进程在使用Socket之前，
必须调用 WSAStartup() 来加载并初始化ws2_32.dll。
在使用完毕后，必须调用 WSACleanup() 来释放资源。这是一个强制性的步骤。
 (#else): Linux没有这个要求*/

// 初始化socket系统
socket_error_t socket_system_init(void)
{
#ifdef _WIN32
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        return SOCKET_ERROR_INIT_FAILED;
    }
#endif
    return SOCKET_SUCCESS;
}

// 清理socket系统
void socket_system_cleanup(void)
{
#ifdef _WIN32
    WSACleanup();
#endif
}

// 创建socket
socket_t socket_create(int domain, int type, int protocol)
{
    socket_t sock = socket(domain, type, protocol);
#ifdef _WIN32
    if (sock == INVALID_SOCKET)
    {
        return SOCKET_INVALID;
    }
#else
    if (sock < 0)
    {
        return SOCKET_INVALID;
    }
#endif
    return sock;
}

// 绑定socket
socket_error_t socket_bind(socket_t sock, const struct sockaddr *addr, socklen_t addrlen)
{
    if (bind(sock, addr, addrlen) != 0)
    {
        return SOCKET_ERROR_BIND_FAILED;
    }
    return SOCKET_SUCCESS;
}

// 设置端口重用
socket_error_t socket_set_reuse_addr(socket_t sock, bool enable)
{
    int optval = enable ? 1 : 0;
#ifdef _WIN32
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) != 0)
    {
        return SOCKET_ERROR_SETSOCKOPT_FAILED;
    }
#else
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0)
    {
        return SOCKET_ERROR_SETSOCKOPT_FAILED;
    }
#endif
    return SOCKET_SUCCESS;
}

// 设置非阻塞模式
socket_error_t socket_set_nonblocking(socket_t sock, bool nonblocking)
{
#ifdef _WIN32
    u_long mode = nonblocking ? 1 : 0;
    if (ioctlsocket(sock, FIONBIO, &mode) != 0)
    {
        return SOCKET_ERROR_SET_NONBLOCK_FAILED;
    }
#else
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0)
        return SOCKET_ERROR_SET_NONBLOCK_FAILED;

    if (nonblocking)
    {
        flags |= O_NONBLOCK;
    }
    else
    {
        flags &= ~O_NONBLOCK;
    }

    if (fcntl(sock, F_SETFL, flags) < 0)
    {
        return SOCKET_ERROR_SET_NONBLOCK_FAILED;
    }
#endif
    return SOCKET_SUCCESS;
}

// 关闭socket
socket_error_t socket_close(socket_t sock)
{
#ifdef _WIN32
    if (closesocket(sock) != 0)
    {
        return SOCKET_ERROR_INVALID_SOCKET;
    }
#else
    if (close(sock) != 0)
    {
        return SOCKET_ERROR_INVALID_SOCKET;
    }
#endif
    return SOCKET_SUCCESS;
}

// 发送数据
int socket_send_to(socket_t sock, const void *buf, size_t len, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    return sendto(sock, buf, len, 0, dest_addr, addrlen);
}

// 接收数据
int socket_recv_from(socket_t sock, void *buf, size_t len, struct sockaddr *src_addr, socklen_t *addrlen)
{
    return recvfrom(sock, buf, len, 0, src_addr, addrlen);
}

// 事件监听
socket_error_t socket_poll(socket_poll_t *fds, int nfds, int timeout)
{
#ifdef _WIN32
    // 使用select实现，因为WSAPoll在某些环境下可能不可用
    fd_set readfds, writefds;
    struct timeval tv;
    struct timeval *ptv = NULL;

    if (timeout >= 0)
    {
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        ptv = &tv;
    }

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    socket_t max_fd = 0;
    for (int i = 0; i < nfds; i++)
    {
        if (fds[i].events & SOCKET_EVENT_READ)
        {
            FD_SET(fds[i].fd, &readfds);
        }
        if (fds[i].events & SOCKET_EVENT_WRITE)
        {
            FD_SET(fds[i].fd, &writefds);
        }
        if (fds[i].fd > max_fd)
        {
            max_fd = fds[i].fd;
        }
    }

    int ret = select(max_fd + 1, &readfds, &writefds, NULL, ptv);
    if (ret < 0)
    {
        return SOCKET_ERROR_POLL_FAILED;
    }

    for (int i = 0; i < nfds; i++)
    {
        fds[i].revents = 0;
        if (FD_ISSET(fds[i].fd, &readfds))
        {
            fds[i].revents |= SOCKET_EVENT_READ;
        }
        if (FD_ISSET(fds[i].fd, &writefds))
        {
            fds[i].revents |= SOCKET_EVENT_WRITE;
        }
    }
#else
    struct pollfd *linux_fds = malloc(nfds * sizeof(struct pollfd));
    if (!linux_fds)
        return SOCKET_ERROR_POLL_FAILED;

    for (int i = 0; i < nfds; i++)
    {
        linux_fds[i].fd = fds[i].fd;
        linux_fds[i].events = 0;
        if (fds[i].events & SOCKET_EVENT_READ)
            linux_fds[i].events |= POLLIN;
        if (fds[i].events & SOCKET_EVENT_WRITE)
            linux_fds[i].events |= POLLOUT;
    }

    int ret = poll(linux_fds, nfds, timeout);
    if (ret < 0)
    {
        free(linux_fds);
        return SOCKET_ERROR_POLL_FAILED;
    }

    for (int i = 0; i < nfds; i++)
    {
        fds[i].revents = 0;
        if (linux_fds[i].revents & POLLIN)
            fds[i].revents |= SOCKET_EVENT_READ;
        if (linux_fds[i].revents & POLLOUT)
            fds[i].revents |= SOCKET_EVENT_WRITE;
    }
    free(linux_fds);
#endif
    return SOCKET_SUCCESS;
}

// 获取错误描述
const char *socket_get_error_string(socket_error_t error)
{
    switch (error)
    {
    case SOCKET_SUCCESS:
        return "Success";
    case SOCKET_ERROR_INIT_FAILED:
        return "Socket system initialization failed";
    case SOCKET_ERROR_CREATE_FAILED:
        return "Socket creation failed";
    case SOCKET_ERROR_BIND_FAILED:
        return "Socket bind failed";
    case SOCKET_ERROR_SETSOCKOPT_FAILED:
        return "Socket setsockopt failed";
    case SOCKET_ERROR_SEND_FAILED:
        return "Socket send failed";
    case SOCKET_ERROR_RECV_FAILED:
        return "Socket receive failed";
    case SOCKET_ERROR_SET_NONBLOCK_FAILED:
        return "Socket set non-blocking failed";
    case SOCKET_ERROR_POLL_FAILED:
        return "Socket poll failed";
    case SOCKET_ERROR_INVALID_SOCKET:
        return "Invalid socket";
    default:
        return "Unknown error";
    }
}

// 获取最后的系统错误
int socket_get_last_error(void)
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}