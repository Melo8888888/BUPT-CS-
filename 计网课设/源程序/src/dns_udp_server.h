#pragma once
#define DNS_PORT 53
#define BUFFER_SIZE 1500 // DNS报文的最大尺寸
#define ID_LIST_SIZE 128 // ID映射表大小

#include "socket_wrapper.h"
#include "dns_protocol_structs.h" // 为Dns_Mes类型

extern int socketMode;        // 阻塞/非阻塞模式
extern socket_t clientSocket; // 客户端socket
extern socket_t serverSocket; // 服务端socket
extern struct sockaddr_in clientAddress;
extern struct sockaddr_in serverAddress;
extern socklen_t addressLength;

extern int clientPort;         // 客户端端口号
extern char *dnsServerAddress; // 远程主机（BUPT的DNS服务器）

extern int islisten;

void initializeSocket();
void closeSocketServer();
void setNonBlockingMode();
void setBlockingMode();
void receive_client();
void parse_and_cache_dns_response(Dns_Mes *msg, uint8_t *buffer);
void receive_server();