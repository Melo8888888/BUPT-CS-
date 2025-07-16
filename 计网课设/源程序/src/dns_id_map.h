#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
// Windows 下使用 Critical Section 代替互斥锁
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

#define MAX_ID_SIZE 128  // 与ID_LIST_SIZE保持一致
#define ID_EXPIRE_TIME 4 // ID过期时间

typedef struct
{
    uint16_t user_id;                  // 用户ID
    time_t expire_time;                // 过期时间 超时机制
    struct sockaddr_in client_address; // 客户端地址
} ClientSession;

extern ClientSession ID_list[MAX_ID_SIZE]; // 存储客户端会话信息的数组
extern MUTEX_TYPE id_map_mutex;            // ID映射互斥锁

// 基本功能函数
uint16_t reset_id(uint16_t user_id, struct sockaddr_in client_address);
void init_ID_list();

// 新增的辅助函数
int release_id_mapping(uint16_t id);
int get_id_mapping(uint16_t id, uint16_t *user_id, struct sockaddr_in *client_addr);
void print_id_stats();