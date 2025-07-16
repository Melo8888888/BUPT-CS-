#include "dns_id_map.h"
/*
 * DNS ID映射模块 (dns_id_map.c)
 * =================================
 *
 * 功能说明:
 * 本模块为DNS中继服务器提供客户端ID映射管理。当多个客户端同时发送DNS查询时，
 * 需要为每个查询分配唯一的内部ID，并维护与原始客户端ID的映射关系，确保响应
 * 能够正确路由回对应的客户端。
 *
 * 使用流程:
 * 1. 客户端查询 -> reset_id()分配内部ID
 * 2. 转发到上游DNS服务器(使用内部ID)
 * 3. 收到响应 -> get_id_mapping()获取原始客户端
 * 4. 响应路由回客户端 -> release_id_mapping()释放资源
 *
 * 概述:
 * 本模块实现了DNS中继服务器中的ID映射管理机制。在DNS中继场景下，客户端发送DNS查询
 * 到中继服务器，中继服务器需要将查询转发给上游DNS服务器。由于可能存在多个客户端同时
 * 查询，且它们的DNS查询ID可能冲突，因此需要一个ID映射机制来：
 * 1. 为每个客户端查询分配唯一的内部ID
 * 2. 维护客户端原始ID与内部ID的映射关系
 * 3. 当收到上游服务器响应时，能够正确路由回原始客户端
 *
 *
 * 数据结构:
 * - ClientSession数组：固定大小的ID映射表(MAX_ID_SIZE=128)
 * - 每个条目包含：用户原始ID、客户端地址、过期时间
 * - 使用数组索引作为内部分配的ID，简化查找逻辑
 *
 * 并发安全:
 * - id_map_mutex：全局互斥锁保护所有ID操作
 * - 所有公共函数都使用MUTEX_LOCK/MUTEX_UNLOCK包装
 * - 支持多线程
 *
 * ID分配策略:
 * 1. 优先分配完全空闲的槽位(user_id=0, expire_time=0)
 * 2. 如无空闲槽位，则查找已过期的槽位进行复用
 * 3. 如仍无可用槽位，返回UINT16_MAX表示分配失败
 *
 * 过期机制:
 * - 每个ID映射都有expire_time时间戳
 * - 默认过期时间为ID_EXPIRE_TIME秒(4秒)
 * - 过期的映射可以被新请求复用
 * - 支持主动释放和自动过期两种回收方式
 *
 * 使用示例:
 * 1. 客户端A(192.168.1.100)发送查询ID=1234到中继服务器
 * 2. 中继服务器调用reset_id(1234, client_addr)分配内部ID=5
 * 3. 中继服务器用ID=5向上游DNS服务器发送查询
 * 4. 上游服务器回复ID=5的响应
 * 5. 中继服务器调用get_id_mapping(5)获取原始客户端信息
 * 6. 中继服务器将响应的ID改回1234，发送给客户端A
 * 7. 调用release_id_mapping(5)释放映射资源
 *
 * 性能特征:
 * - 时间复杂度：O(MAX_ID_SIZE) = O(128) 近似O(1)
 * - 空间复杂度：O(MAX_ID_SIZE) = 固定内存占用
 * - 并发性能：受互斥锁保护，支持多线程安全访问
 
 * 核心特性:
 * - 线程安全的ID分配和管理
 * - 自动过期回收机制(4秒超时)
 * - O(1)时间复杂度的查找性能
 * - 128个并发ID支持
 *
 * 主要函数:
 * - reset_id(): 为客户端查询分配新的内部ID
 * - get_id_mapping(): 根据内部ID查找原始客户端信息
 * - release_id_mapping(): 主动释放ID映射
 * - print_id_stats(): 打印统计信息
 */







// 全局互斥锁
// id_map_mutex现在在dnsrelay_main.c中定义

uint16_t reset_id(uint16_t user_id, struct sockaddr_in client_address)
{
    MUTEX_LOCK(&id_map_mutex);

    time_t current_time = time(NULL);
    uint16_t allocated_id = UINT16_MAX;

    // 首先查找完全空闲的槽位（从未使用过的）
    for (uint16_t i = 0; i < MAX_ID_SIZE; i++)
    {
        if (ID_list[i].user_id == 0 && ID_list[i].expire_time == 0)
        {
            ID_list[i].user_id = user_id;
            ID_list[i].client_address = client_address;
            ID_list[i].expire_time = current_time + ID_EXPIRE_TIME;
            allocated_id = i;
            break;
        }
    }

    // 如果没有找到完全空闲的槽位，查找过期的槽位
    if (allocated_id == UINT16_MAX)
    {
        for (uint16_t i = 0; i < MAX_ID_SIZE; i++)
        {
            if (ID_list[i].expire_time < current_time)
            {
                // 清理过期槽位并分配
                ID_list[i].user_id = user_id;
                ID_list[i].client_address = client_address;
                ID_list[i].expire_time = current_time + ID_EXPIRE_TIME;
                allocated_id = i;
                break;
            }
        }
    }

    MUTEX_UNLOCK(&id_map_mutex);
    return allocated_id;
}

void init_ID_list()
{
    MUTEX_INIT(&id_map_mutex);

    for (int i = 0; i < MAX_ID_SIZE; i++)
    {
        ID_list[i].user_id = 0;
        ID_list[i].expire_time = 0;
        memset(&(ID_list[i].client_address), 0, sizeof(struct sockaddr_in));
    }
}
//映射回去 收到上级的回复 给  客户端
int get_id_mapping(uint16_t id, uint16_t *user_id, struct sockaddr_in *client_addr)
{
    if (id >= MAX_ID_SIZE || user_id == NULL || client_addr == NULL)
    {
        return 0; // 无效参数
    }

    MUTEX_LOCK(&id_map_mutex);

    time_t current_time = time(NULL);
    int result = 0;

    // 检查ID是否有效且未过期
    if (ID_list[id].expire_time >= current_time && ID_list[id].user_id != 0)
    {
        *user_id = ID_list[id].user_id;
        *client_addr = ID_list[id].client_address;
        result = 1; // 成功
    }

    MUTEX_UNLOCK(&id_map_mutex);
    return result;
}

int release_id_mapping(uint16_t id)
{
    if (id >= MAX_ID_SIZE)
    {
        return 0; // 无效ID
    }

    MUTEX_LOCK(&id_map_mutex);

    // 清除ID映射
    ID_list[id].user_id = 0;
    ID_list[id].expire_time = 0;
    memset(&(ID_list[id].client_address), 0, sizeof(struct sockaddr_in));

    MUTEX_UNLOCK(&id_map_mutex);
    return 1; // 成功
}

void print_id_stats()
{
    MUTEX_LOCK(&id_map_mutex);

    time_t current_time = time(NULL);
    int active_count = 0;
    int expired_count = 0;

    for (int i = 0; i < MAX_ID_SIZE; i++)
    {
        if (ID_list[i].user_id != 0)
        {
            if (ID_list[i].expire_time >= current_time)
            {
                active_count++;
            }
            else
            {
                expired_count++;
            }
        }
    }

    printf("ID Mapping Stats: Active=%d, Expired=%d, Total=%d\n",
           active_count, expired_count, MAX_ID_SIZE);

    MUTEX_UNLOCK(&id_map_mutex);
}
