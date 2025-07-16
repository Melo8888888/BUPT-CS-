#include "dns_protocol_structs.h"
#include "dns_cache_lru.h"
#include "domain_trie_map.h"
#include "dns_udp_server.h"
#include "dns_id_map.h"
#include "socket_wrapper.h"
#include "config_and_host.h"

// 统一定义全局变量
// 来自 dns_cache_lru.h - 新的O(1) LRU缓存
// cache_instance在dns_cache_lru.c中定义

// 来自 domain_trie_map.h
trie_node Trie[MAX_SIZE];
int list_size = 0;

// 来自 dns_udp_server.h
int socketMode = 0; // 默认为非阻塞模式
socket_t clientSocket = SOCKET_INVALID;
socket_t serverSocket = SOCKET_INVALID;
struct sockaddr_in clientAddress;
struct sockaddr_in serverAddress;
// 以下变量在 dns_udp_server.c 中已经定义，这里不再重复定义
// socklen_t addressLength;
// int clientPort;
// char *dnsServerAddress;
int islisten = 0; // 默认不监听

// 来自 config_and_host.h - 部分变量在 config_and_host.c 中已定义
char IPAddr[DNS_RR_NAME_MAX_SIZE];
char domain[DNS_RR_NAME_MAX_SIZE];
// 以下变量在 config_and_host.c 中已定义，不需要在这里定义
// char *host_path;
// char *LOG_PATH;
// int debug_mode;
// int log_mode;

// 来自 dns_id_map.h
ClientSession ID_list[MAX_ID_SIZE];
MUTEX_TYPE id_map_mutex;