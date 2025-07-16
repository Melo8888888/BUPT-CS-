#pragma once
/*
 * 2024/6/26
 * ���ļ����������в��������Լ��������־��,����������������Լ�һ�����������в����ĺ����ӿڡ�
 */
#include "dns_protocol_structs.h"

extern char IPAddr[DNS_RR_NAME_MAX_SIZE];
extern char domain[DNS_RR_NAME_MAX_SIZE];
extern char *host_path; // HOST�ļ�Ŀ¼
extern char *LOG_PATH;  // ��־�ļ�Ŀ¼

extern int debug_mode;
extern int log_mode;

void init(int argc, char *argv[]);

void get_config(int argc, char *argv[]);

void print_help_info();

void write_log(char *domain, uint8_t *ip_addr);

void read_host();

void get_host_info(FILE *ptr);
