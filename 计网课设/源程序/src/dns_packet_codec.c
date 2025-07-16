/*
���ļ�����ʵ��DNS���Ľṹ�����ִ�֮���ת���ȱ��Ĳ���
*/

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef _WIN32
#include <winsock2.h> // Windows: Ϊntohs, ntohl, htons, htonl
#else
#include <arpa/inet.h> // Linux/Unix: Ϊntohs, ntohl, htons, htonl
#endif
#include "config_and_host.h"
#include "dns_packet_codec.h"
#include "dns_debug_print.h"
#include "dns_packet_codec.h"
#include "config_and_host.h"
#include "dns_debug_print.h"

// �ӻ������л�ȡָ��������λ��8, 16, 32����������Ӧ��ֵ
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
        return ntohs(val); // �����ֽ���ת��Ϊ�����ֽ���
    }
    if (bits == 32)
    {
        uint32_t val;
        memcpy(&val, *buffer, 4);
        *buffer += 4;
        return ntohl(val); // �����ֽ���ת��Ϊ�����ֽ���
    }
    return 0; // Ĭ�Ϸ���ֵ��������뾯��
}

// �����յ���DNS���Ĳ��洢��msg�ṹ����
int string_to_dnsstruct(Dns_Mes *pmsg, uint8_t *buffer, uint8_t *start)
{
    // ���ٿռ�
    pmsg->header = malloc(sizeof(Dns_Header));
    if (pmsg->header == NULL)
    {
        return DNS_PARSE_ERROR_MEMORY;
    }

    pmsg->question = NULL; // ��ʼ��ΪNULL��ͨ���������
    pmsg->answer = NULL;   // ��ʼ��ΪNULL��ͨ���������

    if (debug_mode == 1)
        printf("�յ��ı������£�\n");

    // ��ȡ����ͷ
    buffer = get_dnsheader(pmsg, buffer); // bufferָ���ȡ�걨ͷ��ĵ�ַ
    if (debug_mode == 1)
        print_header(pmsg);

    // ��ȡѯ������
    buffer = get_dnsquestion(pmsg, buffer, start); // bufferָ���ȡ��ѯ�����ݺ�ĵ�ַ
    if (buffer == NULL)
    {
        free(pmsg->header);
        return DNS_PARSE_ERROR_FORMAT;
    }
    if (debug_mode == 1)
        print_question(pmsg);

    // ��ȡӦ������
    buffer = get_dnsanswer(pmsg, buffer, start); // bufferָ���ȡ��Ӧ�����ݺ�ĵ�ַ
    if (buffer == NULL)
    {
        // �ͷ��ѷ�����ڴ�
        free(pmsg->header);
        // ����Ӧ���ͷ�question������Ϊ�˼���ʡ��
        return DNS_PARSE_ERROR_FORMAT;
    }
    if (debug_mode == 1)
        print_answer(pmsg);

    return DNS_PARSE_SUCCESS;
}

// �ӻ������ж�ȡDNS����ͷ��Ϣ���洢��msg�ṹ����
uint8_t *get_dnsheader(Dns_Mes *msg, uint8_t *buffer)
{
    msg->header->id = read_bits(&buffer, 16); // ��ȡID

    uint16_t val = read_bits(&buffer, 16); // ��ȡ��־�ֶ�

    // ������־�ֶεĸ�������
    msg->header->qr = (val & QR_MASK) >> 15;
    msg->header->opcode = (val & OPCODE_MASK) >> 11;
    msg->header->aa = (val & AA_MASK) >> 10;
    msg->header->tc = (val & TC_MASK) >> 9;
    msg->header->rd = (val & RD_MASK) >> 8;
    msg->header->ra = (val & RA_MASK) >> 7;
    msg->header->rcode = (val & RCODE_MASK) >> 0;

    // ��ȡ���������ش�����Ȩ����¼�������Ӽ�¼��
    msg->header->qdcount = read_bits(&buffer, 16);
    msg->header->ancount = read_bits(&buffer, 16);
    msg->header->nscount = read_bits(&buffer, 16);
    msg->header->arcount = read_bits(&buffer, 16);

    return buffer;
}

// �ӻ������ж�ȡDNS��ѯ���Ⲣ�洢��msg�ṹ����
uint8_t *get_dnsquestion(Dns_Mes *msg, uint8_t *buffer, uint8_t *start)
{
    int i;
    for (i = 0; i < msg->header->qdcount; ++i)
    {
        char name[DNS_RR_NAME_MAX_SIZE] = {0};
        Dns_Question *p = malloc(sizeof(Dns_Question));
        if (p == NULL)
        {
            return NULL; // �ڴ����ʧ��
        }

        // �ӻ������л�ȡ��ѯ����
        buffer = get_domain(buffer, name, start);
        if (buffer == NULL)
        {
            free(p);
            return NULL; // ��������ʧ��
        }

        // �����ڴ沢��������
        p->q_name = malloc(strlen(name) + 1);
        if (p->q_name == NULL)
        {
            free(p);
            return NULL; // �ڴ����ʧ��
        }
        memcpy(p->q_name, name, strlen(name) + 1);

        p->q_type = read_bits(&buffer, 16);  // ��ȡ��ѯ����
        p->q_class = read_bits(&buffer, 16); // ��ȡ��ѯ��

        // ����������뵽���������ͷ��
        p->next = msg->question;
        msg->question = p;
    }

    return buffer;
}

// �ӻ������ж�ȡDNS�ش���Ϣ���洢��msg�ṹ����
uint8_t *get_dnsanswer(Dns_Mes *msg, uint8_t *buffer, uint8_t *start)
{
    int i;
    for (i = 0; i < msg->header->ancount; ++i)
    {
        char name[DNS_RR_NAME_MAX_SIZE] = {0};
        Dns_rr *p = malloc(sizeof(Dns_rr));
        if (p == NULL)
        {
            return NULL; // �ڴ����ʧ��
        }

        // �ӻ������л�ȡ�ش�����
        buffer = get_domain(buffer, name, start);
        if (buffer == NULL)
        {
            free(p);
            return NULL; // ��������ʧ��
        }

        // �����ڴ沢��������
        p->name = malloc(strlen(name) + 1);
        if (p->name == NULL)
        {
            free(p);
            return NULL; // �ڴ����ʧ��
        }
        memcpy(p->name, name, strlen(name) + 1);

        p->type = read_bits(&buffer, 16);      // ��ȡ��Դ��¼����
        p->rr_class = read_bits(&buffer, 16);  // ��ȡ��Դ��¼��
        p->ttl = read_bits(&buffer, 32);       // ��ȡTTLֵ
        p->rd_length = read_bits(&buffer, 16); // ��ȡ��Դ���ݳ���

        // ���ݲ�ͬ�����ͽ��д�����Դ����
        switch (p->type)
        {
        case DNS_TYPE_A: // IPv4��ַ
            for (int j = 0; j < 4; j++)
            {
                p->rd_data.a_record.IP_addr[j] = read_bits(&buffer, 8);
            }
            break;
        case DNS_TYPE_AAAA: // IPv6��ַ
            for (int j = 0; j < 16; j++)
            {
                p->rd_data.aaaa_record.IP_addr[j] = read_bits(&buffer, 8);
            }
            break;
        case DNS_TYPE_CNAME: // CNAME��¼
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
        case DNS_TYPE_MX: // MX��¼
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
        case DNS_TYPE_TXT: // TXT��¼
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
        case DNS_TYPE_SOA: // SOA��¼
            // ʹ�ð�ȫ���ַ������ȼ���
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
        default:                    // �������͵ļ�¼
            buffer += p->rd_length; // ������֧�ֵ���Դ����
            break;
        }

        // ���»ش���뵽�ش������ͷ��
        p->next = msg->answer;
        msg->answer = p;
    }
    return buffer;
}

// �ӻ������л�ȡ����
uint8_t *get_domain(uint8_t *buffer, char *name, uint8_t *start)
{
    uint8_t *ptr = buffer;
    int name_pos = 0;
    int jumped = 0;
    uint8_t *ret_ptr = NULL;
    int jump_count = 0;
    const int MAX_JUMPS = 10;

    name[0] = '\0'; // ��ʼ��Ϊ���ַ���

    while (*ptr != 0 && jump_count < MAX_JUMPS)
    {
        // ����Ƿ�Ϊѹ��ָ�� (ǰ��λΪ11)
        if ((*ptr & 0xC0) == 0xC0)
        {
            jump_count++;
            if (jump_count >= MAX_JUMPS)
            {
                return NULL; // ��⵽���ܵ�ѭ��
            }

            // ����ƫ���������ǰ��λ������һ�ֽ����
            uint16_t offset = ((*ptr & 0x3F) << 8) | *(ptr + 1);

            // ����ǵ�һ����ת����¼����λ��
            if (!jumped)
            {
                ret_ptr = ptr + 2;
                jumped = 1;
            }

            // ��ת��ָ��ƫ��λ��
            ptr = start + offset;
            continue;
        }

        // ��ȡ��ǩ����
        uint8_t label_len = *ptr++;

        // ������ǵ�һ����ǩ����ӵ�ָ���
        if (name_pos > 0)
        {
            name[name_pos++] = '.';
        }

        // ���Ʊ�ǩ����
        for (int i = 0; i < label_len && name_pos < DNS_RR_NAME_MAX_SIZE - 1; i++)
        {
            name[name_pos++] = *ptr++;
        }
    }

    // ���ѭ������Ϊjump_count���޶��˳������ش���
    if (jump_count >= MAX_JUMPS)
    {
        return NULL;
    }

    name[name_pos] = '\0'; // ����ַ���������

    // ����������0�ֽ�
    if (!jumped)
    {
        ret_ptr = ptr + 1;
    }

    return ret_ptr;
}

// ��ָ��������λ��8, 16, 32�����õ���������
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

// ��װ��Ҫ������DNS����
uint8_t *dnsstruct_to_string(Dns_Mes *pmsg, uint8_t *buffer, uint8_t *ip_addr)
{
    // uint8_t *start = buffer;  // ע�͵�δʹ�õı���

    // ��װ��ͷ
    buffer = set_dnsheader(pmsg, buffer, ip_addr);
    // ��װѯ��
    buffer = set_dnsquestion(pmsg, buffer);
    // ��װ�ش�
    buffer = set_dnsanswer(pmsg, buffer, ip_addr);

    return buffer;
}

// ��DNS����ͷ��Ϣת��Ϊ�����ֽ��򲢴洢����������
uint8_t *set_dnsheader(Dns_Mes *msg, uint8_t *buffer, uint8_t *ip_addr)
{
    Dns_Header *header = msg->header;
    header->qr = 1;      // ����Ϊ�ش���
    header->aa = 1;      // ����ΪȨ������������
    header->ra = 1;      // ����Ϊ���õݹ�
    header->ancount = 1; // ���ûش���Ϊ1

    if (ip_addr[0] == 0 && ip_addr[1] == 0 && ip_addr[2] == 0 && ip_addr[3] == 0)
    {
        // ��IP��ַΪ0.0.0.0�����ݿ���ppt����ʾ������������
        header->rcode = 3; // ������Ϊ���ִ���
    }
    else
    {
        header->rcode = 0; // �޲��
    }

    write_bits(&buffer, 16, header->id); // ����ID

    int flags = 0;
    // ���ñ�־�ֶεĸ�������
    flags |= (header->qr << 15) & QR_MASK;
    flags |= (header->opcode << 11) & OPCODE_MASK;
    flags |= (header->aa << 10) & AA_MASK;
    flags |= (header->tc << 9) & TC_MASK;
    flags |= (header->rd << 8) & RD_MASK;
    flags |= (header->ra << 7) & RA_MASK;
    flags |= (header->rcode << 0) & RCODE_MASK;

    write_bits(&buffer, 16, flags);           // ���ñ�־�ֶ�
    write_bits(&buffer, 16, header->qdcount); // ����������
    write_bits(&buffer, 16, header->ancount); // ���ûش���
    write_bits(&buffer, 16, header->nscount); // ����Ȩ����¼��
    write_bits(&buffer, 16, header->arcount); // ���ø��Ӽ�¼��

    return buffer;
}

// ��DNS��ѯ����ת��Ϊ�����ֽ��򲢴洢����������
uint8_t *set_dnsquestion(Dns_Mes *msg, uint8_t *buffer)
{
    for (int i = 0; i < msg->header->qdcount; i++)
    {
        Dns_Question *p = msg->question;
        buffer = set_domain(buffer, p->q_name); // ��������

        write_bits(&buffer, 16, p->q_type);  // ���ò�ѯ����
        write_bits(&buffer, 16, p->q_class); // ���ò�ѯ��

        p = p->next;
    }
    return buffer;
}

// ��DNS�ش���Ϣת��Ϊ�����ֽ��򲢴洢����������
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

// ������ת��Ϊ�����ֽ��򲢴洢����������
uint8_t *set_domain(uint8_t *buffer, char *name)
{
    const uint8_t *ptr = (const uint8_t *)name; // ��ȷ������ת��
    char tmp[DNS_RR_NAME_MAX_SIZE] = {0};
    int i = 0;

    // uint8_t *s = buffer;  // ע�͵�δʹ�õı���

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

// �ͷ�DNS���Ľṹ����ռ���ڴ�
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