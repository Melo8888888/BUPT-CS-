#pragma once
#include "dns_protocol_structs.h"
#include "config_and_host.h"

#define MAX_CACHE 100		// cache���ߴ�
#define HASH_TABLE_SIZE 127 // ��ϣ���С��������

// O(1) LRU Cache�ڵ�ṹ��
typedef struct cache_node
{
	uint8_t IP[4];					   // ʮ����IP��ַ,�ĸ�ʮ������
	char domain[DNS_RR_NAME_MAX_SIZE]; // ����
	uint16_t dns_type;				   // DNS��¼���ͣ���ǰֻ֧��DNS_TYPE_A��
	struct cache_node *prev;		   // ˫������ǰ��ָ��
	struct cache_node *next;		   // ˫��������ָ��
	struct cache_node *hash_next;	   // ��ϣ��ͻ����ָ��
} cache_node_t;

// O(1) LRU��ϣ�������ṹ��
typedef struct
{
	cache_node_t **buckets; // ��ϣͰ����
	cache_node_t *head;		// LRU����ͷ�ڵ�(����)
	cache_node_t *tail;		// LRU����β�ڵ�(����)
	int hash_size;			// ��ϣ���С
	int current_size;		// ��ǰ������Ŀ��
	int max_size;			// ��󻺴���Ŀ��
} lru_hash_cache_t;

// ȫ�ֻ���ʵ��
extern lru_hash_cache_t *cache_instance;

// ��ʼ��cache
void init_cache();

// ��cache�в�ѯĳ���������ض����ͼ�¼
int cache_query(uint8_t *ipv4, char *domain, uint16_t dns_type);

// �����µ�������ip��ָ��DNS��¼���ͣ�
void insert_cache(uint8_t ipv4[4], char *domain, uint16_t dns_type);

// ɾ�����ϵ�������ip
void delete_node();

// ��������Դ
void cache_cleanup();
