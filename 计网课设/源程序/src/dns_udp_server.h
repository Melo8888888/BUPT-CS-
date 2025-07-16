#pragma once
#define DNS_PORT 53
#define BUFFER_SIZE 1500 // DNS���ĵ����ߴ�
#define ID_LIST_SIZE 128 // IDӳ����С

#include "socket_wrapper.h"
#include "dns_protocol_structs.h" // ΪDns_Mes����

extern int socketMode;        // ����/������ģʽ
extern socket_t clientSocket; // �ͻ���socket
extern socket_t serverSocket; // �����socket
extern struct sockaddr_in clientAddress;
extern struct sockaddr_in serverAddress;
extern socklen_t addressLength;

extern int clientPort;         // �ͻ��˶˿ں�
extern char *dnsServerAddress; // Զ��������BUPT��DNS��������

extern int islisten;

void initializeSocket();
void closeSocketServer();
void setNonBlockingMode();
void setBlockingMode();
void receive_client();
void parse_and_cache_dns_response(Dns_Mes *msg, uint8_t *buffer);
void receive_server();