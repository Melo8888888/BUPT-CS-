
/*
 本文件用于实现dns报文的基本结构
     DNS 报文格式如下：
     +---------------------+
     |        Header       | 报文头，DNS_header
     +---------------------+
     |       Question      | 请求，DNS_question
     +---------------------+
     |        Answer       | 回复，DNS_resource_record
     +---------------------+
     |       Authority     | 指向授权域名服务器
     +---------------------+
     |       Additional    | 附加信息
     +---------------------+
 */
 #pragma once
 #include <time.h>
 #include <stdio.h>  // 标准输入输出头文件
 #include <stdlib.h> // 标准库函数头文件，如malloc, free等
 #include <string.h> // 字符串操作函数头文件
 #include <stdint.h> // 定义了整型变量的精确宽度类型

 #ifdef _WIN32
 #pragma warning(disable : 4996) // 禁用编译器警告4996，通常是关于某些函数不安全的警告
 #endif

 // DNS报文的最大长度
 #define DNS_STRING_MAX_SIZE 8192

 // DNS资源记录中域名的最大长度
 #define DNS_RR_NAME_MAX_SIZE 512

 // DNS查询或响应标志，0表示查询，1表示响应
 #define DNS_QR_QUERY 0
 #define DNS_QR_ANSWER 1

 // DNS操作码，表示查询类型
 // 0表示标准查询，1表示反向查询，2表示服务器状态请求
 #define DNS_OPCODE_QUERY 0
 #define DNS_OPCODE_IQUERY 1
 #define DNS_OPCODE_STATUS 2

 // DNS资源记录类型，表示不同类型的DNS记录
 #define DNS_TYPE_A 1      // A记录，表示IPv4地址
 #define DNS_TYPE_NS 2     // NS记录，表示权威名称服务器
 #define DNS_TYPE_CNAME 5  // CNAME记录，表示规范名称
 #define DNS_TYPE_SOA 6    // SOA记录，表示起始授权机构
 #define DNS_TYPE_PTR 12   // PTR记录，表示指针记录
 #define DNS_TYPE_HINFO 13 // HINFO记录，表示主机信息
 #define DNS_TYPE_MINFO 14 // MINFO记录，表示邮件信息
 #define DNS_TYPE_MX 15    // MX记录，表示邮件交换记录
 #define DNS_TYPE_TXT 16   // TXT记录，表示文本记录
 #define DNS_TYPE_AAAA 28  // AAAA记录，表示IPv6地址

 // DNS类，表示地址类型，通常为1，表示因特网
 #define DNS_CLASS_IN 1

 // DNS响应代码，表示查询的返回状态
 // 0表示无错误，3表示名字错误
 #define DNS_RCODE_OK 0
 #define DNS_RCODE_NXDOMAIN 3

 // DNS解析错误码
 #define DNS_PARSE_SUCCESS 0
 #define DNS_PARSE_ERROR_MEMORY 1
 #define DNS_PARSE_ERROR_FORMAT 2
 #define DNS_PARSE_ERROR_LOOP 3




 typedef struct DNS_mes
 {
     Dns_Header *header;
     Dns_Question *question;
     Dns_rr *answer;
     Dns_rr *authority;
     Dns_rr *additional;
 } Dns_Mes;


/**
 * @brief DNS Header 报文头格式框图 (共12字节)
 *
 * 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |                      ID                       | (16 bits) 事务ID
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   | (16 bits) 标志位
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |                    QDCOUNT                    | (16 bits) 问题数
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |                    ANCOUNT                    | (16 bits) 回答数
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |                    NSCOUNT                    | (16 bits) 权威记录数
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |                    ARCOUNT                    | (16 bits) 附加记录数
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *
 * 字段说明:
 * - ID (Identifier): 16位，客户端设置的事务ID，响应中原样返回，用于匹配请求和响应。
 * - QR (Query/Response): 1位，0为查询，1为响应。
 * - Opcode: 4位，0为标准查询，1为反向查询，2为服务器状态请求。
 * - AA (Authoritative Answer): 1位，1表示响应来自权威服务器。
 * - TC (TrunCation): 1位，1表示报文被截断。
 * - RD (Recursion Desired): 1位，1表示客户端希望服务器进行递归查询。
 * - RA (Recursion Available): 1位，1表示服务器支持递归查询。
 * - Z (Reserved): 3位，保留位，必须为0。
 * - RCODE (Response Code): 4位，响应码，0为无错误，3为域名不存在等。
 * - QDCOUNT (Question Count): 16位，问题部分的条目数。
 * - ANCOUNT (Answer Count): 16位，回答部分的资源记录数。
 * - NSCOUNT (Authority Count): 16位，权威部分的资源记录数。
 * - ARCOUNT (Additional Count): 16位，附加部分的资源记录数。
 */

 typedef struct DNS_header
 {
     uint16_t id;

     uint8_t qr : 1;
     uint8_t opcode : 4;
     uint8_t aa : 1;
     uint8_t tc : 1;
     uint8_t rd : 1;

     uint8_t ra : 1;
     uint8_t z : 3;
     uint8_t rcode : 4;

     uint16_t qdcount;
     uint16_t ancount;
     uint16_t nscount;
     uint16_t arcount;
 } Dns_Header;

/**
 * @brief DNS Question 问题部分格式框图
 *
 * +-------------------------------------------------+
 * |                     QNAME                       | (可变长度) 查询的域名
 * +-------------------------------------------------+
 * |                     QTYPE                       | (16 bits) 查询类型 (如: A, AAAA, MX)
 * +-------------------------------------------------+
 * |                     QCLASS                      | (16 bits) 查询类 (通常为 1, 表示 IN)
 * +-------------------------------------------------+
 *
 * 字段说明:
 * - QNAME (Query Name): 长度可变的域名字符串。格式为<长度><标签><长度><标签>...<0>。
 * 例如, "www.google.com" 会被编码为 "3www6google3com0"。
 * - QTYPE (Query Type): 16位，查询的资源记录类型。
 * - QCLASS (Query Class): 16位，查询的地址类，通常为1(IN)。
 */
 typedef struct DNS_question
 {
     char *q_name;
     uint16_t q_type;
     uint16_t q_class;
     struct DNS_question *next;
 } Dns_Question;

 // 定义 ResourceData 联合体，用于存储不同类型的资源数据
 union ResourceData
 {
     // A 记录 (IPv4 地址)
     struct
     {
         uint8_t IP_addr[4]; // 4 字节的 IPv4 地址
     } a_record;

     // AAAA 记录 (IPv6 地址)
     struct
     {
         uint8_t IP_addr[16]; // 16 字节的 IPv6 地址
     } aaaa_record;

     // SOA 记录 (权威记录的起始)
     struct
     {
         char *MName;
         char *RName;
         uint32_t serial;
         uint32_t refresh;
         uint32_t retry;
         uint32_t expire;
         uint32_t minimum;
     } soa_record;

     // CNAME 记录 (规范名称)
     struct
     {
         char *cname; // 规范名称
     } cname_record;

     // MX 记录 (邮件交换)
     struct
     {
         uint16_t preference; // 优先级，值越小优先级越高
         char *exchange;       // 邮件交换服务器域名
     } mx_record;

     // TXT 记录 (文本记录)
     struct
     {
         char *text; // 文本内容
     } txt_record;
 };

/**
 * @brief DNS Resource Record  (用于Answer, Authority, Additional部分)
 *
 * +-------------------------------------------------+
 * |                      NAME                       | (可变长度) 域名
 * +-------------------------------------------------+
 * |                      TYPE                       | (16 bits) 记录类型 (如: A, CNAME)
 * +-------------------------------------------------+
 * |                     CLASS                       | (16 bits) 记录类 (通常为 1, 表示 IN)
 * +-------------------------------------------------+
 * |                      TTL                        | (32 bits) 生存时间 (秒)
 * +-------------------------------------------------+
 * |                    RDLENGTH                     | (16 bits) RDATA 字段的长度
 * +-------------------------------------------------+
 * |                      RDATA                      | (可变长度) 资源数据
 * +-------------------------------------------------+
 *
 * RDATA (资源数据) 格式示例:
 *
 * 1. A 记录 (TYPE=1)
 * +--+--+--+--+
 * |  IPv4 地址 | (32 bits)
 * +--+--+--+--+
 *
 * 2. AAAA 记录 (TYPE=28)
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |                     IPv6 地址                  | (128 bits)
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *
 * 3. MX 记录 (TYPE=15)
 * +-------------------------------------------------+
 * |                  PREFERENCE                   | (16 bits) 优先级
 * +-------------------------------------------------+
 * |                   EXCHANGE                    | (可变长度) 邮件服务器域名
 * +-------------------------------------------------+
 *
 * 4. CNAME 记录 (TYPE=5)
 * +-------------------------------------------------+
 * |                     CNAME                       | (可变长度) 规范域名
 * +-------------------------------------------------+
 */
 typedef struct DNS_resource_record
 {
     char *name;
     uint16_t type;
     uint16_t rr_class;
     uint32_t ttl;
     uint16_t rd_length;
     union ResourceData rd_data;
     struct DNS_resource_record *next;
 } Dns_rr;

/**
 * @brief DNS 完整报文结构体框图
 *
 * 此结构体将DNS报文的各个部分组织在一起。
 *
 * +--------------------------------------------+
 * |              Dns_Header *header            | 指向DNS头部的指针
 * +--------------------------------------------+
 * |            Dns_Question *question          | 指向问题部分链表头部的指针
 * +--------------------------------------------+
 * |               Dns_rr *answer               | 指向回答部分链表头部的指针
 * +--------------------------------------------+
 * |              Dns_rr *authority             | 指向权威部分链表头部的指针
 * +--------------------------------------------+
 * |             Dns_rr *additional             | 指向附加部分链表头部的指针
 * +--------------------------------------------+
 *
 * - header:     指向唯一的DNS头部。
 * - question:   指向问题链表的第一个节点。通常只有一个问题。
 * - answer:     指向回答资源记录链表的第一个节点。
 * - authority:  指向权威资源记录链表的第一个节点。
 * - additional: 指向附加资源记录链表的第一个节点。
 */

