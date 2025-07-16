#define _GNU_SOURCE // 为strdup，必须在所有头文件之前定义
#include <string.h>
#include <stdlib.h> // 为malloc, free, exit
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
    socketMode = 0; // 默认非阻塞模式
    islisten = 0;   // 起始不监听

    /* 获取程序运行参数 */
    get_config(argc, argv);

    /* 初始化socket */
    initializeSocket();

    /* 初始化ID映射表 */
    init_ID_list();

    /* 初始化缓存 */
    init_cache();

    /* 初始化HOST文件 */
    read_host();

    /* 如果是无参数模式，显示ID统计信息 */
    if (argc == 1)
    {
        print_id_stats();
        printf("\n=== DNS服务器已启动，等待查询... ===\n\n");
    }
}

/* 读取程序命令参数 */
void get_config(int argc, char *argv[])
{
    // 如果没有参数，设置默认的调试模式
    if (argc == 1)
    {
        debug_mode = 1;
        log_mode = 1;
        LOG_PATH = "./log.txt";
        printf("=== 默认模式：调试+日志+系统信息 ===\n");
        printf("调试模式已开启 (Debug mode enabled).\n");
        printf("日志记录已开启，日志文件: %s (Logging enabled, log file: %s).\n", LOG_PATH, LOG_PATH);
        printf("--- 系统基本信息 (System Information) ---\n");
        printf("Hosts 文件路径 (Hosts file path): %s\n", host_path);
        printf("远程 DNS 服务器地址 (Remote DNS server address): %s\n", dnsServerAddress ? dnsServerAddress : "10.3.9.45, BUPT DNS (default)");
        printf("运行模式 (Mode): %s\n", socketMode == 0 ? "非阻塞 (nonblock)" : "poll (阻塞模拟)");
        printf("-----------------------------------------\n");
        // 注意：ID统计将在ID系统初始化后显示
        return; // 直接返回，使用默认设置
    }

    // 只在有参数时打印帮助信息
    print_help_info();

    for (int index = 1; index < argc; index++)
    {
        if (strcmp(argv[index], "-d") == 0)
        { // 开启调试模式
            debug_mode = 1;
            printf("调试模式已开启 (Debug mode enabled).\n");
        }
        else if (strcmp(argv[index], "-l") == 0) {
            log_mode = 1;
            LOG_PATH = "./log.txt"; // Initialize LOG_PATH when log_mode is set
            printf("日志记录已开启，日志文件: %s (Logging enabled, log file: %s).\n", LOG_PATH, LOG_PATH);
        }
        else if (strcmp(argv[index], "-i") == 0) {  //打印系统基本信息
            printf("--- 系统基本信息 (System Information) ---\n");
            printf("Hosts 文件路径 (Hosts file path): %s\n", host_path);
            printf("远程 DNS 服务器地址 (Remote DNS server address): %s\n", dnsServerAddress ? dnsServerAddress : "10.3.9.45, BUPT DNS (default)");
            printf("运行模式 (Mode): %s\n", socketMode == 0 ? "非阻塞 (nonblock)" : "poll (阻塞模拟)"); // Added description for mode
            printf("-----------------------------------------\n");
        }
        else if (strcmp(argv[index], "-s") == 0 && index + 1 < argc) {    //设置远程DNS服务器地址
            // 检查是否有足够的参数
            if (dnsServerAddress) free(dnsServerAddress);  // 避免内存泄漏
            dnsServerAddress = strdup(argv[++index]);  // 使用 strdup 来复制字符串
            printf("远程 DNS 服务器地址已设置为 (Remote DNS server address set to): %s\n", dnsServerAddress);
        }
        else if (strcmp(argv[index], "-m") == 0 && index + 1 < argc) {    //设置程序的运行模式
            socketMode = atoi(argv[++index]);  // 直接使用 atoi 转换模式数字
            printf("程序运行模式已设置为 (Program mode set to): %s\n", socketMode == 0 ? "非阻塞 (nonblock)" : "poll (阻塞模拟)");
        }
        else {
            // 提示未知参数或参数错误
            printf("未知或无效参数: %s (Unknown or invalid argument: %s)\n", argv[index], argv[index]);
            print_help_info(); // 打印帮助信息以便用户了解正确用法
            exit(1); // 参数错误，退出程序
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
    printf("  魏生辉 徐鹤松\n");
    printf("\n");
    fflush(stdout); // 确保输出被刷新
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
        printf("%d 加载成功！\n\n", num);
}

void write_log(char *domain, uint8_t *ip_addr)
{
    FILE *fp = fopen("./log.txt", "a");
    if (fp == NULL)
    {
        if (debug_mode)
            printf("File open failed.\n");

        return; // 提前退出，避免深层嵌套
    }

    // if (debug_mode) {
    //     printf("File open succeeded.\n");
    // }

    // 获取并格式化当前时间
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);
    char timeString[100];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTime);
    fprintf(fp, "%s  %s  ", timeString, domain);

    // 根据 IP 地址是否存在写入不同的内容
    if (ip_addr != NULL)
    {
        fprintf(fp, "%d.%d.%d.%d\n", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    }
    else
    {
        fprintf(fp, "Not found in local. Returned from remote DNS server.\n");
    }

    // 刷新缓冲区并关闭文件
    fflush(fp);
    fclose(fp);
}