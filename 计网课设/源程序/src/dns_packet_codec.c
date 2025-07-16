/*
本文件用于实现DNS报文结构体与字串之间的转换等报文操作
*/

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef _WIN32
#include <winsock2.h> // Windows: 为ntohs, ntohl, htons, htonl
#else
#include <arpa/inet.h> // Linux/Unix: 为ntohs, ntohl, htons, htonl
#endif
#include "config_and_host.h"
#include "dns_packet_codec.h"
#include "dns_debug_print.h"
#include "dns_packet_codec.h"
#include "config_and_host.h"
#include "dns_debug_print.h"

// 从缓冲区中获取指定数量的位（8, 16, 32）并返回相应的值
size_t read_bits(uint8_t **buffer, int bits)
{
    if (bits == 8)
    {
        uint8_t val;
        memcpy(&val, *buffer, 1);
        *buffer += 1;
        return val;
    }
    if (bits == 16)
    {
        uint16_t val;
        memcpy(&val, *buffer, 2);
        *buffer += 2;
        return ntohs(val); // 网络字节序转换为主机字节序
    }
    if (bits == 32)
    {
        uint32_t val;
        memcpy(&val, *buffer, 4);
        *buffer += 4;
        return ntohl(val); // 网络字节序转换为主机字节序
    }
    return 0; // 默认返回值，避免编译警告
}

// 解析收到的DNS报文并存储到msg结构体中
int string_to_dnsstruct(Dns_Mes *pmsg, uint8_t *buffer, uint8_t *start)
{
    // 开辟空间
    pmsg->header = malloc(sizeof(Dns_Header));
    if (pmsg->header == NULL)
    {
        return DNS_PARSE_ERROR_MEMORY;
    }

    pmsg->question = NULL; // 初始化为NULL，通过链表管理
    pmsg->answer = NULL;   // 初始化为NULL，通过链表管理

    if (debug_mode == 1)
        printf("收到的报文如下：\n");

    // 获取报文头
    buffer = get_dnsheader(pmsg, buffer); // buffer指向读取完报头后的地址
    if (debug_mode == 1)
        print_header(pmsg);

    // 获取询问内容
    buffer = get_dnsquestion(pmsg, buffer, start); // buffer指向读取完询问内容后的地址
    if (buffer == NULL)
    {
        free(pmsg->header);
        return DNS_PARSE_ERROR_FORMAT;
    }
    if (debug_mode == 1)
        print_question(pmsg);

    // 获取应答内容
    buffer = get_dnsanswer(pmsg, buffer, start); // buffer指向读取完应答内容后的地址
    if (buffer == NULL)
    {
        // 释放已分配的内存
        free(pmsg->header);
        // 这里应该释放question链表，但为了简化先省略
        return DNS_PARSE_ERROR_FORMAT;
    }
    if (debug_mode == 1)
        print_answer(pmsg);

    return DNS_PARSE_SUCCESS;
}

// 从缓冲区中读取DNS报文头信息并存储到msg结构体中
uint8_t *get_dnsheader(Dns_Mes *msg, uint8_t *buffer)
{
    msg->header->id = read_bits(&buffer, 16); // 获取ID

    uint16_t val = read_bits(&buffer, 16); // 获取标志字段

    // 解析标志字段的各个部分
    msg->header->qr = (val & QR_MASK) >> 15;
    msg->header->opcode = (val & OPCODE_MASK) >> 11;
    msg->header->aa = (val & AA_MASK) >> 10;
    msg->header->tc = (val & TC_MASK) >> 9;
    msg->header->rd = (val & RD_MASK) >> 8;
    msg->header->ra = (val & RA_MASK) >> 7;
    msg->header->rcode = (val & RCODE_MASK) >> 0;

    // 获取问题数、回答数、权威记录数、附加记录数
    msg->header->qdcount = read_bits(&buffer, 16);
    msg->header->ancount = read_bits(&buffer, 16);
    msg->header->nscount = read_bits(&buffer, 16);
    msg->header->arcount = read_bits(&buffer, 16);

    return buffer;
}

// 从缓冲区中读取DNS查询问题并存储到msg结构体中
uint8_t *get_dnsquestion(Dns_Mes *msg, uint8_t *buffer, uint8_t *start)
{
    int i;
    for (i = 0; i < msg->header->qdcount; ++i)
    {
        char name[DNS_RR_NAME_MAX_SIZE] = {0};
        Dns_Question *p = malloc(sizeof(Dns_Question));
        if (p == NULL)
        {
            return NULL; // 内存分配失败
        }

        // 从缓冲区中获取查询域名
        buffer = get_domain(buffer, name, start);
        if (buffer == NULL)
        {
            free(p);
            return NULL; // 域名解析失败
        }

        // 分配内存并复制域名
        p->q_name = malloc(strlen(name) + 1);
        if (p->q_name == NULL)
        {
            free(p);
            return NULL; // 内存分配失败
        }
        memcpy(p->q_name, name, strlen(name) + 1);

        p->q_type = read_bits(&buffer, 16);  // 获取查询类型
        p->q_class = read_bits(&buffer, 16); // 获取查询类

        // 将新问题插入到问题链表的头部
        p->next = msg->question;
        msg->question = p;
    }

    return buffer;
}

// 从缓冲区中读取DNS回答信息并存储到msg结构体中
uint8_t *get_dnsanswer(Dns_Mes *msg, uint8_t *buffer, uint8_t *start)
{
    int i;
    for (i = 0; i < msg->header->ancount; ++i)
    {
        char name[DNS_RR_NAME_MAX_SIZE] = {0};
        Dns_rr *p = malloc(sizeof(Dns_rr));
        if (p == NULL)
        {
            return NULL; // 内存分配失败
        }

        // 从缓冲区中获取回答域名
        buffer = get_domain(buffer, name, start);
        if (buffer == NULL)
        {
            free(p);
            return NULL; // 域名解析失败
        }

        // 分配内存并复制域名
        p->name = malloc(strlen(name) + 1);
        if (p->name == NULL)
        {
            free(p);
            return NULL; // 内存分配失败
        }
        memcpy(p->name, name, strlen(name) + 1);

        p->type = read_bits(&buffer, 16);      // 获取资源记录类型
        p->rr_class = read_bits(&buffer, 16);  // 获取资源记录类
        p->ttl = read_bits(&buffer, 32);       // 获取TTL值
        p->rd_length = read_bits(&buffer, 16); // 获取资源数据长度

        // 根据不同的类型进行处理资源数据
        switch (p->type)
        {
        case DNS_TYPE_A: // IPv4地址
            for (int j = 0; j < 4; j++)
            {
                p->rd_data.a_record.IP_addr[j] = read_bits(&buffer, 8);
            }
            break;
        case DNS_TYPE_AAAA: // IPv6地址
            for (int j = 0; j < 16; j++)
            {
                p->rd_data.aaaa_record.IP_addr[j] = read_bits(&buffer, 8);
            }
            break;
        case DNS_TYPE_CNAME: // CNAME记录
            p->rd_data.cname_record.cname = malloc(DNS_RR_NAME_MAX_SIZE);
            if (p->rd_data.cname_record.cname == NULL)
            {
                free(p->name);
                free(p);
                return NULL;
            }
            buffer = get_domain(buffer, p->rd_data.cname_record.cname, start);
            if (buffer == NULL)
            {
                free(p->rd_data.cname_record.cname);
                free(p->name);
                free(p);
                return NULL;
            }
            break;
        case DNS_TYPE_MX: // MX记录
            p->rd_data.mx_record.preference = read_bits(&buffer, 16);
            p->rd_data.mx_record.exchange = malloc(DNS_RR_NAME_MAX_SIZE);
            if (p->rd_data.mx_record.exchange == NULL)
            {
                free(p->name);
                free(p);
                return NULL;
            }
            buffer = get_domain(buffer, p->rd_data.mx_record.exchange, start);
            if (buffer == NULL)
            {
                free(p->rd_data.mx_record.exchange);
                free(p->name);
                free(p);
                return NULL;
            }
            break;
        case DNS_TYPE_TXT: // TXT记录
            p->rd_data.txt_record.text = malloc(p->rd_length + 1);
            if (p->rd_data.txt_record.text == NULL)
            {
                free(p->name);
                free(p);
                return NULL;
            }
            memcpy(p->rd_data.txt_record.text, buffer, p->rd_length);
            p->rd_data.txt_record.text[p->rd_length] = '\0';
            buffer += p->rd_length;
            break;
        case DNS_TYPE_SOA: // SOA记录
            // 使用安全的字符串长度计算
            p->rd_data.soa_record.MName = malloc(DNS_RR_NAME_MAX_SIZE);
            if (p->rd_data.soa_record.MName == NULL)
            {
                free(p->name);
                free(p);
                return NULL;
            }
            buffer = get_domain(buffer, p->rd_data.soa_record.MName, start);
            if (buffer == NULL)
            {
                free(p->rd_data.soa_record.MName);
                free(p->name);
                free(p);
                return NULL;
            }
            p->rd_data.soa_record.RName = malloc(DNS_RR_NAME_MAX_SIZE);
            if (p->rd_data.soa_record.RName == NULL)
            {
                free(p->rd_data.soa_record.MName);
                free(p->name);
                free(p);
                return NULL;
            }
            buffer = get_domain(buffer, p->rd_data.soa_record.RName, start);
            if (buffer == NULL)
            {
                free(p->rd_data.soa_record.RName);
                free(p->rd_data.soa_record.MName);
                free(p->name);
                free(p);
                return NULL;
            }
            p->rd_data.soa_record.serial = read_bits(&buffer, 32);
            p->rd_data.soa_record.refresh = read_bits(&buffer, 32);
            p->rd_data.soa_record.retry = read_bits(&buffer, 32);
            p->rd_data.soa_record.expire = read_bits(&buffer, 32);
            p->rd_data.soa_record.minimum = read_bits(&buffer, 32);
            break;
        default:                    // 其他类型的记录
            buffer += p->rd_length; // 跳过不支持的资源数据
            break;
        }

        // 将新回答插入到回答链表的头部
        p->next = msg->answer;
        msg->answer = p;
    }
    return buffer;
}

// 从缓冲区中获取域名
uint8_t *get_domain(uint8_t *buffer, char *name, uint8_t *start)
{
    uint8_t *ptr = buffer;
    int name_pos = 0;
    int jumped = 0;
    uint8_t *ret_ptr = NULL;
    int jump_count = 0;
    const int MAX_JUMPS = 10;

    name[0] = '\0'; // 初始化为空字符串

    while (*ptr != 0 && jump_count < MAX_JUMPS)
    {
        // 检查是否为压缩指针 (前两位为11)
        if ((*ptr & 0xC0) == 0xC0)
        {
            jump_count++;
            if (jump_count >= MAX_JUMPS)
            {
                return NULL; // 检测到可能的循环
            }

            // 计算偏移量：清除前两位，与下一字节组合
            uint16_t offset = ((*ptr & 0x3F) << 8) | *(ptr + 1);

            // 如果是第一次跳转，记录返回位置
            if (!jumped)
            {
                ret_ptr = ptr + 2;
                jumped = 1;
            }

            // 跳转到指定偏移位置
            ptr = start + offset;
            continue;
        }

        // 读取标签长度
        uint8_t label_len = *ptr++;

        // 如果不是第一个标签，添加点分隔符
        if (name_pos > 0)
        {
            name[name_pos++] = '.';
        }

        // 复制标签内容
        for (int i = 0; i < label_len && name_pos < DNS_RR_NAME_MAX_SIZE - 1; i++)
        {
            name[name_pos++] = *ptr++;
        }
    }

    // 如果循环是因为jump_count超限而退出，返回错误
    if (jump_count >= MAX_JUMPS)
    {
        return NULL;
    }

    name[name_pos] = '\0'; // 添加字符串结束符

    // 跳过结束的0字节
    if (!jumped)
    {
        ret_ptr = ptr + 1;
    }

    return ret_ptr;
}

// 将指定数量的位（8, 16, 32）设置到缓冲区中
void write_bits(uint8_t **buffer, int bits, int value)
{
    if (bits == 8)
    {
        **buffer = (uint8_t)value;
        (*buffer)++;
    }
    if (bits == 16)
    {
        uint16_t val = htons((uint16_t)value);
        memcpy(*buffer, &val, 2);
        *buffer += 2;
    }
    if (bits == 32)
    {
        uint32_t val = htonl(value);
        memcpy(*buffer, &val, 4);
        *buffer += 4;
    }
}

// 组装将要发出的DNS报文
uint8_t *dnsstruct_to_string(Dns_Mes *pmsg, uint8_t *buffer, uint8_t *ip_addr)
{
    // uint8_t *start = buffer;  // 注释掉未使用的变量

    // 组装报头
    buffer = set_dnsheader(pmsg, buffer, ip_addr);
    // 组装询问
    buffer = set_dnsquestion(pmsg, buffer);
    // 组装回答
    buffer = set_dnsanswer(pmsg, buffer, ip_addr);

    return buffer;
}

// 将DNS报文头信息转换为网络字节序并存储到缓冲区中
uint8_t *set_dnsheader(Dns_Mes *msg, uint8_t *buffer, uint8_t *ip_addr)
{
    Dns_Header *header = msg->header;
    header->qr = 1;      // 设置为回答报文
    header->aa = 1;      // 设置为权威域名服务器
    header->ra = 1;      // 设置为可用递归
    header->ancount = 1; // 设置回答数为1

    if (ip_addr[0] == 0 && ip_addr[1] == 0 && ip_addr[2] == 0 && ip_addr[3] == 0)
    {
        // 若IP地址为0.0.0.0，根据课设ppt，表示该域名被屏蔽
        header->rcode = 3; // 我们认为名字错误
    }
    else
    {
        header->rcode = 0; // 无差错
    }

    write_bits(&buffer, 16, header->id); // 设置ID

    int flags = 0;
    // 设置标志字段的各个部分
    flags |= (header->qr << 15) & QR_MASK;
    flags |= (header->opcode << 11) & OPCODE_MASK;
    flags |= (header->aa << 10) & AA_MASK;
    flags |= (header->tc << 9) & TC_MASK;
    flags |= (header->rd << 8) & RD_MASK;
    flags |= (header->ra << 7) & RA_MASK;
    flags |= (header->rcode << 0) & RCODE_MASK;

    write_bits(&buffer, 16, flags);           // 设置标志字段
    write_bits(&buffer, 16, header->qdcount); // 设置问题数
    write_bits(&buffer, 16, header->ancount); // 设置回答数
    write_bits(&buffer, 16, header->nscount); // 设置权威记录数
    write_bits(&buffer, 16, header->arcount); // 设置附加记录数

    return buffer;
}

// 将DNS查询问题转换为网络字节序并存储到缓冲区中
uint8_t *set_dnsquestion(Dns_Mes *msg, uint8_t *buffer)
{
    for (int i = 0; i < msg->header->qdcount; i++)
    {
        Dns_Question *p = msg->question;
        buffer = set_domain(buffer, p->q_name); // 设置域名

        write_bits(&buffer, 16, p->q_type);  // 设置查询类型
        write_bits(&buffer, 16, p->q_class); // 设置查询类

        p = p->next;
    }
    return buffer;
}

// 将DNS回答信息转换为网络字节序并存储到缓冲区中
uint8_t *set_dnsanswer(Dns_Mes *msg, uint8_t *buffer, uint8_t *ip_addr)
{
    int i;

    buffer = set_domain(buffer, msg->question->q_name);

    write_bits(&buffer, 16, 1); // type
    write_bits(&buffer, 16, 1); // rr_class
    write_bits(&buffer, 32, 4); // ttl
    write_bits(&buffer, 16, 4); // rd_length

    for (i = 0; i < 4; i++)
    {
        *buffer = ip_addr[i];
        buffer++;
    }

    return buffer;
}

// 将域名转换为网络字节序并存储到缓冲区中
uint8_t *set_domain(uint8_t *buffer, char *name)
{
    const uint8_t *ptr = (const uint8_t *)name; // 正确的类型转换
    char tmp[DNS_RR_NAME_MAX_SIZE] = {0};
    int i = 0;

    // uint8_t *s = buffer;  // 注释掉未使用的变量

    while (1)
    {
        if (*ptr == 0)
        {
            *buffer = i;
            buffer++;
            memcpy(buffer, tmp, i);
            buffer += i;

            *buffer = 0;
            buffer++;
            break;
        }
        else if (*ptr != '.')
        {
            tmp[i++] = *ptr;
        }
        else if (*ptr == '.')
        {
            *buffer = i;
            buffer++;
            memcpy(buffer, tmp, i);
            buffer += i;
            memset(tmp, 0, sizeof(tmp));
            i = 0;
        }
        ptr++;
    }

    return buffer;
}

// 释放DNS报文结构体所占的内存
void free_message(Dns_Mes *msg)
{
    free(msg->header);

    Dns_Question *p = msg->question;
    while (p)
    {
        Dns_Question *tmp = p;
        p = p->next;
        free(tmp);
    }

    Dns_rr *q = msg->answer;
    while (q)
    {
        Dns_rr *tmp = q;
        q = q->next;
        free(tmp);
    }

    free(msg);
}