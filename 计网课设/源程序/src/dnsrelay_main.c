#include "dns_protocol_structs.h"
#include "config_and_host.h"
#include "dns_udp_server.h"
#include "dns_cache_lru.h"
#include "dns_id_map.h"
#include "domain_trie_map.h"

// 全局变量在globals.c中定义

int main(int argc, char *argv[])
{
    /* 初始化系统 */
    init(argc, argv);

    /* 以非阻塞模式运行 */
    if (socketMode == 0)
    {
        setNonBlockingMode();
    }

    /* 以阻塞模式（poll）运行 */
    if (socketMode == 1)
    {
        setBlockingMode();
    }

    /* 关闭连接 */
    closeSocketServer();

    return 0;
}