#include "dns_protocol_structs.h"
#include "config_and_host.h"
#include "dns_udp_server.h"
#include "dns_cache_lru.h"
#include "dns_id_map.h"
#include "domain_trie_map.h"

// ȫ�ֱ�����globals.c�ж���

int main(int argc, char *argv[])
{
    /* ��ʼ��ϵͳ */
    init(argc, argv);

    /* �Է�����ģʽ���� */
    if (socketMode == 0)
    {
        setNonBlockingMode();
    }

    /* ������ģʽ��poll������ */
    if (socketMode == 1)
    {
        setBlockingMode();
    }

    /* �ر����� */
    closeSocketServer();

    return 0;
}