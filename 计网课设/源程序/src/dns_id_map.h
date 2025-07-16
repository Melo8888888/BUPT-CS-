#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
// Windows ��ʹ�� Critical Section ���滥����
#include <Windows.h>
#define MUTEX_TYPE CRITICAL_SECTION
#define MUTEX_INIT(m) InitializeCriticalSection(m)
#define MUTEX_LOCK(m) EnterCriticalSection(m)
#define MUTEX_UNLOCK(m) LeaveCriticalSection(m)
#define MUTEX_DESTROY(m) DeleteCriticalSection(m)
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#define MUTEX_TYPE pthread_mutex_t
#define MUTEX_INIT(m) pthread_mutex_init(m, NULL)
#define MUTEX_LOCK(m) pthread_mutex_lock(m)
#define MUTEX_UNLOCK(m) pthread_mutex_unlock(m)
#define MUTEX_DESTROY(m) pthread_mutex_destroy(m)
#endif

#include "dns_cache_lru.h"

#define MAX_ID_SIZE 128  // ��ID_LIST_SIZE����һ��
#define ID_EXPIRE_TIME 4 // ID����ʱ��

typedef struct
{
    uint16_t user_id;                  // �û�ID
    time_t expire_time;                // ����ʱ�� ��ʱ����
    struct sockaddr_in client_address; // �ͻ��˵�ַ
} ClientSession;

extern ClientSession ID_list[MAX_ID_SIZE]; // �洢�ͻ��˻Ự��Ϣ������
extern MUTEX_TYPE id_map_mutex;            // IDӳ�以����

// �������ܺ���
uint16_t reset_id(uint16_t user_id, struct sockaddr_in client_address);
void init_ID_list();

// �����ĸ�������
int release_id_mapping(uint16_t id);
int get_id_mapping(uint16_t id, uint16_t *user_id, struct sockaddr_in *client_addr);
void print_id_stats();