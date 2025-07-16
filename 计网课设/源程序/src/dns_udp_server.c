#define _BSD_SOURCE
#define _DEFAULT_SOURCE // 需要定义这些以使usleep可用
#include "dns_udp_server.h"
#include "config_and_host.h"
#include "domain_trie_map.h"
#include "dns_id_map.h"
#include <unistd.h> // 为usleep函数
#include <stdlib.h> // 为malloc, free
#include "dns_packet_codec.h"

// ANSI 颜色代码，用于在控制台输出不同颜色的文本。
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define RESET "\x1B[0m"

// 客户端端口和地址长度变量
int clientPort;
socklen_t addressLength;
// DNS服务器的IP地址
char *dnsServerAddress = "10.3.9.4";

void initializeSocket()
{
    addressLength = sizeof(struct sockaddr_in);

    // 统一的socket系统初始化
    socket_error_t init_result = socket_system_init();
    if (init_result != SOCKET_SUCCESS)
    {
        fprintf(stderr, "Socket system initialization failed: %s\n",
                socket_get_error_string(init_result));
        exit(1);
    }

    // 创建客户端和服务器socket
    clientSocket = socket_create(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == SOCKET_INVALID)
    {
        fprintf(stderr, "Error creating client socket: %d\n", socket_get_last_error());
        socket_system_cleanup();
        exit(1);
    }

    serverSocket = socket_create(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == SOCKET_INVALID)
    {
        fprintf(stderr, "Error creating server socket: %d\n", socket_get_last_error());
        socket_close(clientSocket);
        socket_system_cleanup();
        exit(1);
    }

    // 初始化地址结构
    memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = INADDR_ANY;
    clientAddress.sin_port = htons(DNS_PORT);

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(dnsServerAddress);
    serverAddress.sin_port = htons(DNS_PORT);

    // 设置端口重用
    socket_error_t reuse_result = socket_set_reuse_addr(clientSocket, true);
    if (reuse_result != SOCKET_SUCCESS)
    {
        fprintf(stderr, "setsockopt(SO_REUSEADDR) failed: %s\n",
                socket_get_error_string(reuse_result));
        socket_close(clientSocket);
        socket_close(serverSocket);
        socket_system_cleanup();
        exit(1);
    }

    // 绑定客户端socket
    socket_error_t bind_result = socket_bind(clientSocket,
                                             (struct sockaddr *)&clientAddress,
                                             sizeof(clientAddress));
    if (bind_result != SOCKET_SUCCESS)
    {
        fprintf(stderr, "Bind failed: %s\n", socket_get_error_string(bind_result));
        socket_close(clientSocket);
        socket_close(serverSocket);
        socket_system_cleanup();
        exit(1);
    }

    // 打印服务器信息
    printf("DNS server: %s\n", dnsServerAddress);
    printf("Listening on port %d\n", DNS_PORT);
}

// 关闭套接字并清理socket系统
void closeSocketServer()
{
    socket_close(clientSocket);
    socket_close(serverSocket);
    socket_system_cleanup();
}

// 非阻塞模式
void setNonBlockingMode()
{
    // 尝试将服务器和客户端套接字设置为非阻塞模式
    socket_error_t server_result = socket_set_nonblocking(serverSocket, true);
    socket_error_t client_result = socket_set_nonblocking(clientSocket, true);

    if (server_result != SOCKET_SUCCESS || client_result != SOCKET_SUCCESS)
    {
        fprintf(stderr, "Failed to set non-blocking mode: %s\n\n",
                socket_get_error_string(server_result != SOCKET_SUCCESS ? server_result : client_result));
        socket_close(serverSocket);
        socket_close(clientSocket);
        socket_system_cleanup();
        exit(EXIT_FAILURE);
    }

    // 在非阻塞模式下，循环接收来自客户端和服务器的数据
    while (1)
    {
        receive_client(); // 接收来自客户端的数据
        receive_server(); // 接收来自服务器的数据

        // 避免CPU使用率过高，添加微小延迟
#ifdef _WIN32
        Sleep(1); // Windows: 1毫秒延迟
#else
        usleep(1000); // Linux: 1毫秒延迟
#endif
    }
}

// 使用统一poll接口进行阻塞模式下的事件监听
void setBlockingMode()
{
    // 使用统一poll接口进行阻塞模式下的事件监听
    socket_poll_t fds[2];              // 用于存储需要监听的套接字和事件类型
    fds[0].fd = clientSocket;          // 设置需要监听的套接字
    fds[0].events = SOCKET_EVENT_READ; // 指定监听的事件类型为 "可读"
    fds[1].fd = serverSocket;
    fds[1].events = SOCKET_EVENT_READ;

    // 循环等待直到有事件发生
    while (1)
    {
        // 调用统一poll接口等待套接字上的事件。-1 参数表示无限等待，直到有事件发生。
        socket_error_t poll_result = socket_poll(fds, 2, -1);
        if (poll_result != SOCKET_SUCCESS)
        {
            fprintf(stderr, "Poll failed: %s\n", socket_get_error_string(poll_result));
            socket_close(serverSocket);
            socket_close(clientSocket);
            socket_system_cleanup();
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & SOCKET_EVENT_READ)
        {
            receive_client(); // 处理客户端事件
        }
        if (fds[1].revents & SOCKET_EVENT_READ)
        {
            receive_server(); // 处理服务器事件
        }
    }
}

// 接收来自客户端的数据并进行处理
void receive_client()
{
    uint8_t buffer[BUFFER_SIZE];     // 接收的报文
    uint8_t buffer_new[BUFFER_SIZE]; // 回复给客户端的报文
    Dns_Mes msg;                     // 报文结构体
    uint8_t ip_addr[4] = {0};        // 查询域名得到的IP地址
    int msg_size = -1;               // 报文大小
    int is_found = 0;                // 是否查到

    msg_size = socket_recv_from(clientSocket, buffer, sizeof(buffer), (struct sockaddr *)&clientAddress, &addressLength);

    // 只有真正接收到数据时才处理和打印调试信息
    if (msg_size > 0)
    {
        if (debug_mode == 1)
            printf(RED "收到来自客户端的DNS查询报文\n\n" RESET);

        uint8_t *start = buffer;
        /* 解析客户端发来的DNS报文，将其保存到msg结构体内 */
        string_to_dnsstruct(&msg, buffer, start);

        /* 从cache缓存查找 - 只查找A记录(IPv4) */
        if (msg.question->q_type == DNS_TYPE_A)
        {
            is_found = cache_query(ip_addr, msg.question->q_name, DNS_TYPE_A);
        }
        else
        {
            is_found = 0; // 非A记录不使用缓存，直接转发
            if (debug_mode == 1)
                printf(RED "Non-A record query (Type: %d), skip cache lookup\n\n" RESET, msg.question->q_type);
        }

        /* 若cache未查到，则从host文件查找 */
        if (is_found == 0)
        {
            if (debug_mode == 1)
                printf(RED "在cache里没有找到这个域名,尝试在本地hosts文件里查找.\n\n" RESET);

            // 只有A记录查询才搜索hosts文件
            if (msg.question->q_type == DNS_TYPE_A)
            {
                is_found = query_node(msg.question->q_name, ip_addr);
            }
            else
            {
                is_found = 0; // 非A记录不查询hosts文件
                if (debug_mode == 1)
                    printf(RED "Non-A record query (Type: %d), skip hosts file lookup\n\n" RESET, msg.question->q_type);
            }

            /* 若未查到，则上交远程DNS服务器处理*/
            if (is_found == 0)
            {
                /* 给将要发给远程DNS服务器的包分配新ID */
                uint16_t newID = reset_id(msg.header->id, clientAddress);
                if (newID == UINT16_MAX)
                {
                    printf(RED "Error: ID list is full, cannot forward query.\n\n" RESET);
                }
                else
                {
                    // 修改报文中的ID为新分配的ID (网络字节序)
                    uint16_t network_newID = htons(newID);
                    memcpy(buffer, &network_newID, sizeof(uint16_t));

                    socket_send_to(serverSocket, buffer, msg_size, (struct sockaddr *)&serverAddress, addressLength);
                    if (debug_mode == 1)
                        printf(RED "本地hosts文件里没有找到这个域名,分配新的ID %u,发给上级DNS服务器.\n\n" RESET, newID);
                }
                return;
            }
        }

        uint8_t *end;
        end = dnsstruct_to_string(&msg, buffer_new, ip_addr);

        int len = end - buffer_new;

        /* 将DNS应答报文发回客户端 */
        socket_send_to(clientSocket, buffer_new, len, (struct sockaddr *)&clientAddress, addressLength);

       
    }
}

// 解析DNS响应中的A记录并存入缓存
void parse_and_cache_dns_response(Dns_Mes *msg, uint8_t *buffer)
{
    (void)buffer; // 防止未使用参数警告
    
    if (!msg || !msg->answer || !msg->question)
    {
        return;
    }

    // 只处理A记录响应
    if (msg->question->q_type != DNS_TYPE_A)
    {
        if (debug_mode == 1)
            printf(RED "Non-A record response (Type: %d), skip caching\n\n" RESET, msg->question->q_type);
        return;
    }

    // 检查是否有成功的A记录答案
    if (msg->header->ancount > 0 && msg->header->rcode == DNS_RCODE_OK)
    {
        Dns_rr *answer = msg->answer;

        while (answer)
        {
            // 找到A记录
            if (answer->type == DNS_TYPE_A && answer->rd_length == 4)
            {
                uint8_t ipv4[4];
                memcpy(ipv4, answer->rd_data.a_record.IP_addr, 4);

                // 存入缓存
                insert_cache(ipv4, msg->question->q_name, DNS_TYPE_A);

                if (debug_mode == 1)
                    printf(RED "Remote DNS response cached! Domain: %s, IP: %d.%d.%d.%d\n\n" RESET,
                           msg->question->q_name, ipv4[0], ipv4[1], ipv4[2], ipv4[3]);

                break; // 只缓存第一个A记录
            }
            answer = answer->next;
        }
    }
    else
    {
        if (debug_mode == 1)
            printf(RED "DNS response not successful (rcode: %d, ancount: %d), not caching\n\n" RESET,
                   msg->header->rcode, msg->header->ancount);
    }
}

// 接收来自远程DNS服务器的响应并转发给客户端
void receive_server()
{
    uint8_t buffer[BUFFER_SIZE]; // 接收的报文
    Dns_Mes msg;
    int msg_size = -1; // 报文大小

    /* 尝试接收远程DNS服务器发来的DNS应答报文 */
    msg_size = socket_recv_from(serverSocket, buffer, sizeof(buffer), (struct sockaddr *)&serverAddress, &addressLength);

    /* 处理接收到的DNS应答报文 */
    if (msg_size > 0)
    {
        if (debug_mode == 1)
            printf(RED "收到上级DNS服务器发来的DNS应答报文\n\n" RESET);

        string_to_dnsstruct(&msg, buffer, buffer);

        /* ID转换和安全检查 */
        uint16_t response_id = msg.header->id;
        uint16_t original_user_id;
        struct sockaddr_in original_client_addr;

        // 使用新的安全函数获取映射信息
        if (get_id_mapping(response_id, &original_user_id, &original_client_addr))
        {
            // 解析并缓存DNS响应中的A记录
            parse_and_cache_dns_response(&msg, buffer);

            // 将响应ID改回原始用户ID
            uint16_t network_user_id = htons(original_user_id);
            memcpy(buffer, &network_user_id, sizeof(uint16_t));

            // 释放ID映射
            release_id_mapping(response_id);

            // 发送到原始客户端地址
            socket_send_to(clientSocket, buffer, msg_size, (struct sockaddr *)&original_client_addr, addressLength);

            if (debug_mode == 1)
                printf(GREEN "Response forwarded to client. ID %u -> %u\n\n" RESET, response_id, original_user_id);

            if (log_mode == 1)
            {
                write_log(msg.question->q_name, NULL);
            }
        }
        else
        {
            if (debug_mode == 1)
                fprintf(stderr, RED "Error: Invalid or expired ID mapping for response ID %u\n\n" RESET, response_id);
        }
    }
}