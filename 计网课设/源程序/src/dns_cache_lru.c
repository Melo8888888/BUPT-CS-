#include "dns_cache_lru.h"
/*
该模块通过结合两种数据结构来实现高效性：

哈希表 : 
用于实现 O(1) 时间复杂度的快速查询。
每个域名通过哈希函数计算得到一个唯一的桶(bucket)索引，
从而避免了遍历整个缓存来查找数据。即使发生哈希碰撞，
也只需遍历一个很短的链表。

双向链表: 
用于实现 LRU 淘汰策略。所有缓存节点都存在于一个双向链表中。

链表头部 的节点是 最近被使用 的。
链表尾部 的节点是 最久未被使用 的 
*/
// ANSI color codes
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define RESET "\x1B[0m"

// 全局缓存实例
lru_hash_cache_t *cache_instance = NULL;

// DJB2哈希函数
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
 * @brief 从LRU双向链表中移除一个节点
 * @details 这是一个 O(1) 操作，因为它直接通过节点的 prev 和 next 指针来修改链表结构，
 * 无需遍历。这是实现高效LRU策略的基础。
 */
static void remove_from_list(cache_node_t *node)
{
    if (node->prev)
    {
        node->prev->next = node->next;
    }
    else
    {
        // 如果节点是头节点
        cache_instance->head = node->next;
    }

    if (node->next)
    {
        node->next->prev = node->prev;
    }
    else
    {
        // 如果节点是尾节点
        cache_instance->tail = node->prev;
    }
}

/**
 * @brief 将一个节点添加到LRU双向链表的头部
 * @details 这是一个 O(1) 操作。新插入的节点或者最近被访问的节点会被放到头部，
 * 表示它是“最新”的。
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
        // 如果链表为空，新节点既是头也是尾
        cache_instance->tail = node;
    }
}

/**
 * @brief 将一个已存在的节点移动到LRU链表的头部
 * @details 这是LRU策略的核心！！！！
 * 当一个缓存条目被命中时，调用此函数。
 * 它首先将节点从当前位置移除，然后添加到头部，标记为“最新”。
 * 整个操作由两个 O(1) 的函数组成，因此总时间复杂度也是 O(1)。
 */
static void move_to_head(cache_node_t *node)
{
    if (node == cache_instance->head)
    {
        return; // 已经在头部，无需移动
    }

    // 从当前位置移除
    remove_from_list(node);

    // 添加到头部
    add_to_head(node);
}

// 移除并返回LRU链表尾节点
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
 * @brief 在哈希表中查找节点 (高效查询的关键)
 * @details 1. 使用哈希函数计算 domain 对应的桶(bucket)索引，这是一个 O(L) 操作 (L为域名长度)，
 * 对于长度有限的域名，可近似看作 O(1)。
 * 2. 直接访问哈希表的 bucket，这是一个 O(1) 操作。
 * 3. 如果发生哈希碰撞，遍历该 bucket 对应的链表。理想情况下，链表很短。
 * 综合来看，平均查询时间复杂度接近 O(1)。
 */
static cache_node_t *hash_find(const char *domain, uint16_t dns_type)
{
    unsigned int bucket = hash_function(domain);
    cache_node_t *node = cache_instance->buckets[bucket];

    // 遍历哈希桶中的链表（处理哈希碰撞）
    while (node)
    {
        if (strcmp(node->domain, domain) == 0 && node->dns_type == dns_type)
        {
            return node; // 找到节点
        }
        node = node->hash_next;
    }

    return NULL; // 未找到
}

// 将节点插入哈希表
static void hash_insert(cache_node_t *node)
{
    unsigned int bucket = hash_function(node->domain);
    node->hash_next = cache_instance->buckets[bucket];
    cache_instance->buckets[bucket] = node;
}

// 从哈希表中移除节点
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
 * @brief 淘汰最久未使用的节点 (LRU
 * @details 1. 调用 remove_tail() 从双向链表的尾部获取并移除节点。
 * 这个节点就是最久未使用的(LRU)节点。此操作为 O(1)。
 * 2. 调用 hash_remove() 从哈希表中移除该节点。
 * 3. 释放节点内存。
 * 能精确且高效地淘汰掉最该被淘汰的数据。
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

    // 从哈希表中移除
    hash_remove(lru_node);

    // 释放内存
    free(lru_node);

    // 减少缓存大小
    cache_instance->current_size--;
}

// 初始化缓存
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
 * @brief 缓存查询操作的整体流程
 * @details 1. (高效查询) 调用 hash_find() 在哈希表中查找节点。
 * 2. 如果找到 (缓存命中):
 * - 复制IP地址。
 * - (LRU策略) 调用 move_to_head() 将该节点移动到双向链表的头部，
 * 标记为最近使用。
 * 3. 如果未找到 (缓存未命中)，则返回失败。
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
        // 缓存命中，复制IP地址
        memcpy(ipv4, node->IP, 4);

        // 将节点移动到链表头部，更新其为“最新”
        move_to_head(node);

        if (debug_mode == 1)
        {
            printf(GREEN "Cache HIT! Domain: %s, Type: %s, IP: %d.%d.%d.%d\n\n" RESET,
                   domain,
                   (dns_type == DNS_TYPE_A) ? "A" : "OTHER",
                   ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
        }

        return 1; // 返回命中
    }

    if (debug_mode == 1)
    {
        printf(RED "Cache MISS! Domain: %s, Type: %s\n\n" RESET,
               domain,
               (dns_type == DNS_TYPE_A) ? "A" : "OTHER");
    }

    return 0; // 返回未命中
}

/**
 * @brief 缓存插入操作的整体流程
 * @details 1. 如果记录已存在，则更新其IP并将其移动到双向链表头部。
 * 2. 如果是新记录：
 * - 检查缓存是否已满，如果满则调用 evict_lru_node() 淘汰最久未使用的节点。
 * - 创建新节点。
 * - (高效查询) 调用 hash_insert() 将新节点加入哈希表。
 * - (LRU策略) 调用 add_to_head() 将新节点加入双向链表的头部。
 */
void insert_cache(uint8_t ipv4[4], char *domain, uint16_t dns_type)
{
    if (!cache_instance)
    {
        return;
    }

    // 当前只支持IPv4 A记录
    if (dns_type != DNS_TYPE_A)
    {
        if (debug_mode == 1)
        {
            printf(YELLOW "Cache insert skipped: Only A records (IPv4) are supported. Type: %d\n\n" RESET, dns_type);
        }
        return;
    }

    // 检查是否已存在（更新操作）
    cache_node_t *existing = hash_find(domain, dns_type);
    if (existing)
    {
        // 更新IP地址并移动到头部
        memcpy(existing->IP, ipv4, 4);
        move_to_head(existing);
        return;
    }

    // 如果缓存已满，根据LRU策略删除最久未使用的节点
    if (cache_instance->current_size >= cache_instance->max_size)
    {
        evict_lru_node();
    }

    // 创建新节点
    cache_node_t *new_node = (cache_node_t *)malloc(sizeof(cache_node_t));
    if (!new_node)
    {
        fprintf(stderr, "Failed to allocate memory for cache node\n");
        return;
    }

    // 初始化节点数据
    memcpy(new_node->IP, ipv4, 4);
    memcpy(new_node->domain, domain, strlen(domain) + 1);
    new_node->dns_type = dns_type;
    new_node->prev = NULL;
    new_node->next = NULL;
    new_node->hash_next = NULL;

    // 插入到哈希表
    hash_insert(new_node);

    // 插入到LRU链表头部
    add_to_head(new_node);

    // 增加缓存大小
    cache_instance->current_size++;

    if (debug_mode == 1)
    {
        printf(GREEN "Cache INSERT! Domain: %s, Type: A, IP: %d.%d.%d.%d, Size: %d/%d\n\n" RESET,
               domain, ipv4[0], ipv4[1], ipv4[2], ipv4[3],
               cache_instance->current_size, cache_instance->max_size);
    }
}

// 保持原有接口兼容性
void delete_node()
{
    evict_lru_node();
}

// 清理缓存资源
void cache_cleanup()
{
    if (!cache_instance)
    {
        return;
    }

    // 清理所有缓存节点
    while (cache_instance->head)
    {
        cache_node_t *node = cache_instance->head;
        cache_instance->head = node->next;
        free(node);
    }

    // 清理哈希桶数组
    free(cache_instance->buckets);

    // 清理缓存实例
    free(cache_instance);
    cache_instance = NULL;
}