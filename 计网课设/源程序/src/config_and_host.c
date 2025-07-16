#define _GNU_SOURCE // Ϊstrdup������������ͷ�ļ�֮ǰ����
#include <string.h>
#include <stdlib.h> // Ϊmalloc, free, exit
#include "config_and_host.h"
#include "dns_udp_server.h"
#include "dns_cache_lru.h"
#include "dns_id_map.h"
#include "domain_trie_map.h"

char *host_path = "./dnsrelay.txt";
char *LOG_PATH;
int debug_mode = 0;
int log_mode = 0;

void init(int argc, char *argv[])
{
    socketMode = 0; // Ĭ�Ϸ�����ģʽ
    islisten = 0;   // ��ʼ������

    /* ��ȡ�������в��� */
    get_config(argc, argv);

    /* ��ʼ��socket */
    initializeSocket();

    /* ��ʼ��IDӳ��� */
    init_ID_list();

    /* ��ʼ������ */
    init_cache();

    /* ��ʼ��HOST�ļ� */
    read_host();

    /* ������޲���ģʽ����ʾIDͳ����Ϣ */
    if (argc == 1)
    {
        print_id_stats();
        printf("\n=== DNS���������������ȴ���ѯ... ===\n\n");
    }
}

/* ��ȡ����������� */
void get_config(int argc, char *argv[])
{
    // ���û�в���������Ĭ�ϵĵ���ģʽ
    if (argc == 1)
    {
        debug_mode = 1;
        log_mode = 1;
        LOG_PATH = "./log.txt";
        printf("=== Ĭ��ģʽ������+��־+ϵͳ��Ϣ ===\n");
        printf("����ģʽ�ѿ��� (Debug mode enabled).\n");
        printf("��־��¼�ѿ�������־�ļ�: %s (Logging enabled, log file: %s).\n", LOG_PATH, LOG_PATH);
        printf("--- ϵͳ������Ϣ (System Information) ---\n");
        printf("Hosts �ļ�·�� (Hosts file path): %s\n", host_path);
        printf("Զ�� DNS ��������ַ (Remote DNS server address): %s\n", dnsServerAddress ? dnsServerAddress : "10.3.9.45, BUPT DNS (default)");
        printf("����ģʽ (Mode): %s\n", socketMode == 0 ? "������ (nonblock)" : "poll (����ģ��)");
        printf("-----------------------------------------\n");
        // ע�⣺IDͳ�ƽ���IDϵͳ��ʼ������ʾ
        return; // ֱ�ӷ��أ�ʹ��Ĭ������
    }

    // ֻ���в���ʱ��ӡ������Ϣ
    print_help_info();

    for (int index = 1; index < argc; index++)
    {
        if (strcmp(argv[index], "-d") == 0)
        { // ��������ģʽ
            debug_mode = 1;
            printf("����ģʽ�ѿ��� (Debug mode enabled).\n");
        }
        else if (strcmp(argv[index], "-l") == 0) {
            log_mode = 1;
            LOG_PATH = "./log.txt"; // Initialize LOG_PATH when log_mode is set
            printf("��־��¼�ѿ�������־�ļ�: %s (Logging enabled, log file: %s).\n", LOG_PATH, LOG_PATH);
        }
        else if (strcmp(argv[index], "-i") == 0) {  //��ӡϵͳ������Ϣ
            printf("--- ϵͳ������Ϣ (System Information) ---\n");
            printf("Hosts �ļ�·�� (Hosts file path): %s\n", host_path);
            printf("Զ�� DNS ��������ַ (Remote DNS server address): %s\n", dnsServerAddress ? dnsServerAddress : "10.3.9.45, BUPT DNS (default)");
            printf("����ģʽ (Mode): %s\n", socketMode == 0 ? "������ (nonblock)" : "poll (����ģ��)"); // Added description for mode
            printf("-----------------------------------------\n");
        }
        else if (strcmp(argv[index], "-s") == 0 && index + 1 < argc) {    //����Զ��DNS��������ַ
            // ����Ƿ����㹻�Ĳ���
            if (dnsServerAddress) free(dnsServerAddress);  // �����ڴ�й©
            dnsServerAddress = strdup(argv[++index]);  // ʹ�� strdup �������ַ���
            printf("Զ�� DNS ��������ַ������Ϊ (Remote DNS server address set to): %s\n", dnsServerAddress);
        }
        else if (strcmp(argv[index], "-m") == 0 && index + 1 < argc) {    //���ó��������ģʽ
            socketMode = atoi(argv[++index]);  // ֱ��ʹ�� atoi ת��ģʽ����
            printf("��������ģʽ������Ϊ (Program mode set to): %s\n", socketMode == 0 ? "������ (nonblock)" : "poll (����ģ��)");
        }
        else {
            // ��ʾδ֪�������������
            printf("δ֪����Ч����: %s (Unknown or invalid argument: %s)\n", argv[index], argv[index]);
            print_help_info(); // ��ӡ������Ϣ�Ա��û��˽���ȷ�÷�
            exit(1); // ���������˳�����
        }
    }
}

void print_help_info()
{
    printf("+-+-+-+-+-+-+-+-+-+\n");
    printf("|B|U|P|T|         |\n");
    printf("+-+-+-+-+ +-+-+-+-+\n");
    printf("      |D|N|S| |R|\n");
    printf("      +-+-+-+-+ |E|\n");
    printf("            |L|\n");
    printf("            |A|\n");
    printf("            |Y|\n");
    printf("            +-+\n");
    printf("  κ���� �����\n");
    printf("\n");
    fflush(stdout); // ȷ�������ˢ��
}

void read_host()
{
    FILE *host = fopen(host_path, "r");

    if (!host)
    {
        printf("Error! Can not open hosts file!\n\n");
        exit(1);
    }
    get_host_info(host);
}

void get_host_info(FILE *ptr)
{
    int num = 0;
    while (!feof(ptr))
    {
        uint8_t this_ip[4];
        fscanf(ptr, "%s", IPAddr);
        fscanf(ptr, "%s", domain);
        num++;
        TranIP(this_ip, IPAddr);
        insert_node(this_ip, domain);
    }

    if (debug_mode == 1)
        printf("%d ���سɹ���\n\n", num);
}

void write_log(char *domain, uint8_t *ip_addr)
{
    FILE *fp = fopen("./log.txt", "a");
    if (fp == NULL)
    {
        if (debug_mode)
            printf("File open failed.\n");

        return; // ��ǰ�˳����������Ƕ��
    }

    // if (debug_mode) {
    //     printf("File open succeeded.\n");
    // }

    // ��ȡ����ʽ����ǰʱ��
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);
    char timeString[100];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTime);
    fprintf(fp, "%s  %s  ", timeString, domain);

    // ���� IP ��ַ�Ƿ����д�벻ͬ������
    if (ip_addr != NULL)
    {
        fprintf(fp, "%d.%d.%d.%d\n", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    }
    else
    {
        fprintf(fp, "Not found in local. Returned from remote DNS server.\n");
    }

    // ˢ�»��������ر��ļ�
    fflush(fp);
    fclose(fp);
}