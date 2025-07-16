#include "dns_cache_lru.h"
/*
��ģ��ͨ������������ݽṹ��ʵ�ָ�Ч�ԣ�

��ϣ�� : 
����ʵ�� O(1) ʱ�临�ӶȵĿ��ٲ�ѯ��
ÿ������ͨ����ϣ��������õ�һ��Ψһ��Ͱ(bucket)������
�Ӷ������˱��������������������ݡ���ʹ������ϣ��ײ��
Ҳֻ�����һ���̵ܶ�����

˫������: 
����ʵ�� LRU ��̭���ԡ����л���ڵ㶼������һ��˫�������С�

����ͷ�� �Ľڵ��� �����ʹ�� �ġ�
����β�� �Ľڵ��� ���δ��ʹ�� �� 
*/
// ANSI color codes
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define RESET "\x1B[0m"

// ȫ�ֻ���ʵ��
lru_hash_cache_t *cache_instance = NULL;

// DJB2��ϣ����
static unsigned int hash_function(const char *str)
{
	unsigned int hash = 5381;
	int c;
	while ((c = *str++))
	{
		hash = ((hash << 5) + hash) + c; // hash * 33 + c
	}
	return hash % HASH_TABLE_SIZE;
}

/**
 * @brief ��LRU˫���������Ƴ�һ���ڵ�
 * @details ����һ�� O(1) ��������Ϊ��ֱ��ͨ���ڵ�� prev �� next ָ�����޸�����ṹ��
 * �������������ʵ�ָ�ЧLRU���ԵĻ�����
 */
static void remove_from_list(cache_node_t *node)
{
    if (node->prev)
    {
        node->prev->next = node->next;
    }
    else
    {
        // ����ڵ���ͷ�ڵ�
        cache_instance->head = node->next;
    }

    if (node->next)
    {
        node->next->prev = node->prev;
    }
    else
    {
        // ����ڵ���β�ڵ�
        cache_instance->tail = node->prev;
    }
}

/**
 * @brief ��һ���ڵ���ӵ�LRU˫�������ͷ��
 * @details ����һ�� O(1) �������²���Ľڵ������������ʵĽڵ�ᱻ�ŵ�ͷ����
 * ��ʾ���ǡ����¡��ġ�
 */
static void add_to_head(cache_node_t *node)
{
    node->prev = NULL;
    node->next = cache_instance->head;

    if (cache_instance->head)
    {
        cache_instance->head->prev = node;
    }

    cache_instance->head = node;

    if (!cache_instance->tail)
    {
        // �������Ϊ�գ��½ڵ����ͷҲ��β
        cache_instance->tail = node;
    }
}

/**
 * @brief ��һ���Ѵ��ڵĽڵ��ƶ���LRU�����ͷ��
 * @details ����LRU���Եĺ��ģ�������
 * ��һ��������Ŀ������ʱ�����ô˺�����
 * �����Ƚ��ڵ�ӵ�ǰλ���Ƴ���Ȼ����ӵ�ͷ�������Ϊ�����¡���
 * �������������� O(1) �ĺ�����ɣ������ʱ�临�Ӷ�Ҳ�� O(1)��
 */
static void move_to_head(cache_node_t *node)
{
    if (node == cache_instance->head)
    {
        return; // �Ѿ���ͷ���������ƶ�
    }

    // �ӵ�ǰλ���Ƴ�
    remove_from_list(node);

    // ��ӵ�ͷ��
    add_to_head(node);
}

// �Ƴ�������LRU����β�ڵ�
static cache_node_t *remove_tail()
{
    cache_node_t *tail = cache_instance->tail;
    if (!tail)
    {
        return NULL;
    }

    remove_from_list(tail);
    return tail;
}

/**
 * @brief �ڹ�ϣ���в��ҽڵ� (��Ч��ѯ�Ĺؼ�)
 * @details 1. ʹ�ù�ϣ�������� domain ��Ӧ��Ͱ(bucket)����������һ�� O(L) ���� (LΪ��������)��
 * ���ڳ������޵��������ɽ��ƿ��� O(1)��
 * 2. ֱ�ӷ��ʹ�ϣ��� bucket������һ�� O(1) ������
 * 3. ���������ϣ��ײ�������� bucket ��Ӧ��������������£�����̡ܶ�
 * �ۺ�������ƽ����ѯʱ�临�ӶȽӽ� O(1)��
 */
static cache_node_t *hash_find(const char *domain, uint16_t dns_type)
{
    unsigned int bucket = hash_function(domain);
    cache_node_t *node = cache_instance->buckets[bucket];

    // ������ϣͰ�е����������ϣ��ײ��
    while (node)
    {
        if (strcmp(node->domain, domain) == 0 && node->dns_type == dns_type)
        {
            return node; // �ҵ��ڵ�
        }
        node = node->hash_next;
    }

    return NULL; // δ�ҵ�
}

// ���ڵ�����ϣ��
static void hash_insert(cache_node_t *node)
{
    unsigned int bucket = hash_function(node->domain);
    node->hash_next = cache_instance->buckets[bucket];
    cache_instance->buckets[bucket] = node;
}

// �ӹ�ϣ�����Ƴ��ڵ�
static void hash_remove(cache_node_t *node)
{
    unsigned int bucket = hash_function(node->domain);
    cache_node_t *current = cache_instance->buckets[bucket];
    cache_node_t *prev = NULL;

    while (current)
    {
        if (current == node)
        {
            if (prev)
            {
                prev->hash_next = current->hash_next;
            }
            else
            {
                cache_instance->buckets[bucket] = current->hash_next;
            }
            break;
        }
        prev = current;
        current = current->hash_next;
    }
}

/**
 * @brief ��̭���δʹ�õĽڵ� (LRU
 * @details 1. ���� remove_tail() ��˫�������β����ȡ���Ƴ��ڵ㡣
 * ����ڵ�������δʹ�õ�(LRU)�ڵ㡣�˲���Ϊ O(1)��
 * 2. ���� hash_remove() �ӹ�ϣ�����Ƴ��ýڵ㡣
 * 3. �ͷŽڵ��ڴ档
 * �ܾ�ȷ�Ҹ�Ч����̭����ñ���̭�����ݡ�
 */
static void evict_lru_node()
{
    cache_node_t *lru_node = remove_tail();
    if (!lru_node)
    {
        return;
    }

    if (debug_mode == 1)
    {
        printf(BLUE "Cache EVICT! Domain: %s, Type: %s, IP: %d.%d.%d.%d\n\n" RESET,
               lru_node->domain,
               (lru_node->dns_type == DNS_TYPE_A) ? "A" : "OTHER",
               lru_node->IP[0], lru_node->IP[1], lru_node->IP[2], lru_node->IP[3]);
    }

    // �ӹ�ϣ�����Ƴ�
    hash_remove(lru_node);

    // �ͷ��ڴ�
    free(lru_node);

    // ���ٻ����С
    cache_instance->current_size--;
}

// ��ʼ������
void init_cache()
{
    cache_instance = (lru_hash_cache_t *)malloc(sizeof(lru_hash_cache_t));
    if (!cache_instance)
    {
        fprintf(stderr, "Failed to allocate memory for cache instance\n");
        return;
    }

    cache_instance->buckets = (cache_node_t **)calloc(HASH_TABLE_SIZE, sizeof(cache_node_t *));
    if (!cache_instance->buckets)
    {
        fprintf(stderr, "Failed to allocate memory for hash buckets\n");
        free(cache_instance);
        cache_instance = NULL;
        return;
    }

    cache_instance->head = NULL;
    cache_instance->tail = NULL;
    cache_instance->hash_size = HASH_TABLE_SIZE;
    cache_instance->current_size = 0;
    cache_instance->max_size = MAX_CACHE;
}

/**
 * @brief �����ѯ��������������
 * @details 1. (��Ч��ѯ) ���� hash_find() �ڹ�ϣ���в��ҽڵ㡣
 * 2. ����ҵ� (��������):
 * - ����IP��ַ��
 * - (LRU����) ���� move_to_head() ���ýڵ��ƶ���˫�������ͷ����
 * ���Ϊ���ʹ�á�
 * 3. ���δ�ҵ� (����δ����)���򷵻�ʧ�ܡ�
 */
int cache_query(uint8_t *ipv4, char *domain, uint16_t dns_type)
{
    if (!cache_instance)
    {
        return 0;
    }

    cache_node_t *node = hash_find(domain, dns_type);

    if (node)
    {
        // �������У�����IP��ַ
        memcpy(ipv4, node->IP, 4);

        // ���ڵ��ƶ�������ͷ����������Ϊ�����¡�
        move_to_head(node);

        if (debug_mode == 1)
        {
            printf(GREEN "Cache HIT! Domain: %s, Type: %s, IP: %d.%d.%d.%d\n\n" RESET,
                   domain,
                   (dns_type == DNS_TYPE_A) ? "A" : "OTHER",
                   ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
        }

        return 1; // ��������
    }

    if (debug_mode == 1)
    {
        printf(RED "Cache MISS! Domain: %s, Type: %s\n\n" RESET,
               domain,
               (dns_type == DNS_TYPE_A) ? "A" : "OTHER");
    }

    return 0; // ����δ����
}

/**
 * @brief ��������������������
 * @details 1. �����¼�Ѵ��ڣ��������IP�������ƶ���˫������ͷ����
 * 2. ������¼�¼��
 * - ��黺���Ƿ����������������� evict_lru_node() ��̭���δʹ�õĽڵ㡣
 * - �����½ڵ㡣
 * - (��Ч��ѯ) ���� hash_insert() ���½ڵ�����ϣ��
 * - (LRU����) ���� add_to_head() ���½ڵ����˫�������ͷ����
 */
void insert_cache(uint8_t ipv4[4], char *domain, uint16_t dns_type)
{
    if (!cache_instance)
    {
        return;
    }

    // ��ǰֻ֧��IPv4 A��¼
    if (dns_type != DNS_TYPE_A)
    {
        if (debug_mode == 1)
        {
            printf(YELLOW "Cache insert skipped: Only A records (IPv4) are supported. Type: %d\n\n" RESET, dns_type);
        }
        return;
    }

    // ����Ƿ��Ѵ��ڣ����²�����
    cache_node_t *existing = hash_find(domain, dns_type);
    if (existing)
    {
        // ����IP��ַ���ƶ���ͷ��
        memcpy(existing->IP, ipv4, 4);
        move_to_head(existing);
        return;
    }

    // �����������������LRU����ɾ�����δʹ�õĽڵ�
    if (cache_instance->current_size >= cache_instance->max_size)
    {
        evict_lru_node();
    }

    // �����½ڵ�
    cache_node_t *new_node = (cache_node_t *)malloc(sizeof(cache_node_t));
    if (!new_node)
    {
        fprintf(stderr, "Failed to allocate memory for cache node\n");
        return;
    }

    // ��ʼ���ڵ�����
    memcpy(new_node->IP, ipv4, 4);
    memcpy(new_node->domain, domain, strlen(domain) + 1);
    new_node->dns_type = dns_type;
    new_node->prev = NULL;
    new_node->next = NULL;
    new_node->hash_next = NULL;

    // ���뵽��ϣ��
    hash_insert(new_node);

    // ���뵽LRU����ͷ��
    add_to_head(new_node);

    // ���ӻ����С
    cache_instance->current_size++;

    if (debug_mode == 1)
    {
        printf(GREEN "Cache INSERT! Domain: %s, Type: A, IP: %d.%d.%d.%d, Size: %d/%d\n\n" RESET,
               domain, ipv4[0], ipv4[1], ipv4[2], ipv4[3],
               cache_instance->current_size, cache_instance->max_size);
    }
}

// ����ԭ�нӿڼ�����
void delete_node()
{
    evict_lru_node();
}

// ��������Դ
void cache_cleanup()
{
    if (!cache_instance)
    {
        return;
    }

    // �������л���ڵ�
    while (cache_instance->head)
    {
        cache_node_t *node = cache_instance->head;
        cache_instance->head = node->next;
        free(node);
    }

    // �����ϣͰ����
    free(cache_instance->buckets);

    // ������ʵ��
    free(cache_instance);
    cache_instance = NULL;
}