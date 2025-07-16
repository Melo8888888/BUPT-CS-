#pragma once
#include "dns_protocol_structs.h"
#include "config_and_host.h"

#define MAX_CACHE 100		// cache最大尺寸
#define HASH_TABLE_SIZE 127 // 哈希表大小（质数）

// O(1) LRU Cache节点结构体
typedef struct cache_node
{
	uint8_t IP[4];					   // 十进制IP地址,四个十进制数
	char domain[DNS_RR_NAME_MAX_SIZE]; // 域名
	uint16_t dns_type;				   // DNS记录类型（当前只支持DNS_TYPE_A）
	struct cache_node *prev;		   // 双向链表前驱指针
	struct cache_node *next;		   // 双向链表后继指针
	struct cache_node *hash_next;	   // 哈希冲突链表指针
} cache_node_t;

// O(1) LRU哈希缓存管理结构体
typedef struct
{
	cache_node_t **buckets; // 哈希桶数组
	cache_node_t *head;		// LRU链表头节点(最新)
	cache_node_t *tail;		// LRU链表尾节点(最老)
	int hash_size;			// 哈希表大小
	int current_size;		// 当前缓存条目数
	int max_size;			// 最大缓存条目数
} lru_hash_cache_t;

// 全局缓存实例
extern lru_hash_cache_t *cache_instance;

// 初始化cache
void init_cache();

// 在cache中查询某个域名的特定类型记录
int cache_query(uint8_t *ipv4, char *domain, uint16_t dns_type);

// 插入新的域名及ip（指定DNS记录类型）
void insert_cache(uint8_t ipv4[4], char *domain, uint16_t dns_type);

// 删除最老的域名及ip
void delete_node();

// 清理缓存资源
void cache_cleanup();
