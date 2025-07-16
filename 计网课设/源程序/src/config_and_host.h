#pragma once
/*
 * 2024/6/26
 * 本文件用于命令行参数解析以及输出到日志中,声明了五个参数，以及一个分析命令行参数的函数接口。
 */
#include "dns_protocol_structs.h"

extern char IPAddr[DNS_RR_NAME_MAX_SIZE];
extern char domain[DNS_RR_NAME_MAX_SIZE];
extern char *host_path; // HOST文件目录
extern char *LOG_PATH;  // 日志文件目录

extern int debug_mode;
extern int log_mode;

void init(int argc, char *argv[]);

void get_config(int argc, char *argv[]);

void print_help_info();

void write_log(char *domain, uint8_t *ip_addr);

void read_host();

void get_host_info(FILE *ptr);
