#include "dns_debug_print.h"
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

// ANSI color codes
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define CYAN "\x1B[36m"
#define MAGENTA "\x1B[35m"
#define BRIGHT_WHITE "\x1B[1;37m"
#define BRIGHT_GREEN "\x1B[1;32m"
#define BRIGHT_BLUE "\x1B[1;34m"
#define BRIGHT_YELLOW "\x1B[1;33m"
#define DIM "\x1B[2m"
#define BOLD "\x1B[1m"
#define UNDERLINE "\x1B[4m"
#define RESET "\x1B[0m"

void print_dstring(char *pstring, unsigned int length)
{
    printf(CYAN "========================== RAW DNS PACKET (Hex Dump) ==========================\n" RESET);
    printf(BRIGHT_WHITE "Length: %u bytes\n" RESET, length);

    for (unsigned int i = 0; i < length; i++)
    {
        if (i % 16 == 0)
        {
            printf(DIM "%08X: " RESET, i);
        }
        printf("%02X ", (unsigned char)pstring[i]);
        if ((i + 1) % 8 == 0)
            printf(" ");
        if ((i + 1) % 16 == 0)
        {
            printf(" |");
            for (int j = i - 15; j <= i; j++)
            {
                char c = pstring[j];
                printf("%c", (c >= 32 && c <= 126) ? c : '.');
            }
            printf("|\n");
        }
    }
    if (length % 16 != 0)
    {
        int remaining = 16 - (length % 16);
        for (int i = 0; i < remaining * 3 + (remaining > 8 ? 1 : 0); i++)
            printf(" ");
        printf(" |");
        for (int j = length - (length % 16); j < length; j++)
        {
            char c = pstring[j];
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("|\n");
    }
    printf(CYAN "============================================================================\n" RESET);
}

void print_header(Dns_Mes *msg)
{
    printf(BRIGHT_BLUE "+============================================================================+\n");
    printf("|                               DNS HEADER                                   |\n");
    printf("+============================================================================+\n" RESET);

    printf(BRIGHT_WHITE "Transaction ID: " RESET BRIGHT_YELLOW "%d (0x%04X)" RESET " - Unique identifier for this DNS transaction\n", msg->header->id, msg->header->id);

    printf(BRIGHT_WHITE "Flags:\n" RESET);
    printf("   * QR (Query/Response): " BRIGHT_GREEN "%s" RESET " - %s\n",
           msg->header->qr ? "1 (Response)" : "0 (Query)",
           msg->header->qr ? "This is a DNS response" : "This is a DNS query");
    printf("   * OPCODE: " BRIGHT_GREEN "%d" RESET " - Operation: %s\n",
           msg->header->opcode,
           msg->header->opcode == 0 ? "Standard Query" : msg->header->opcode == 1 ? "Inverse Query"
                                                     : msg->header->opcode == 2   ? "Server Status"
                                                                                  : "Unknown");
    printf("   * AA (Authoritative Answer): " BRIGHT_GREEN "%s" RESET " - %s\n",
           msg->header->aa ? "1 (Yes)" : "0 (No)",
           msg->header->aa ? "Response from authoritative server" : "Response from non-authoritative server");
    printf("   * TC (Truncated): " BRIGHT_GREEN "%s" RESET " - %s\n",
           msg->header->tc ? "1 (Yes)" : "0 (No)",
           msg->header->tc ? "Message was truncated" : "Complete message");
    printf("   * RD (Recursion Desired): " BRIGHT_GREEN "%s" RESET " - %s\n",
           msg->header->rd ? "1 (Yes)" : "0 (No)",
           msg->header->rd ? "Client wants recursive query" : "No recursion requested");
    printf("   * RA (Recursion Available): " BRIGHT_GREEN "%s" RESET " - %s\n",
           msg->header->ra ? "1 (Yes)" : "0 (No)",
           msg->header->ra ? "Server supports recursion" : "Server doesn't support recursion");
    printf("   * RCODE: " BRIGHT_GREEN "%d" RESET " - Status: %s\n",
           msg->header->rcode,
           msg->header->rcode == 0 ? "No Error" : msg->header->rcode == 1 ? "Format Error"
                                              : msg->header->rcode == 2   ? "Server Failure"
                                              : msg->header->rcode == 3   ? "Name Error (NXDOMAIN)"
                                              : msg->header->rcode == 4   ? "Not Implemented"
                                              : msg->header->rcode == 5   ? "Refused"
                                                                          : "Unknown Error");

    printf(BRIGHT_WHITE "Section Counts:\n" RESET);
    printf("   * Questions: " CYAN "%d" RESET " record(s)\n", msg->header->qdcount);
    printf("   * Answers: " CYAN "%d" RESET " record(s)\n", msg->header->ancount);
    printf("   * Authority: " CYAN "%d" RESET " record(s)\n", msg->header->nscount);
    printf("   * Additional: " CYAN "%d" RESET " record(s)\n", msg->header->arcount);
    printf("\n");
}

void print_question(Dns_Mes *msg)
{
    printf(BRIGHT_BLUE "+============================================================================+\n");
    printf("|                              DNS QUESTION                                  |\n");
    printf("+============================================================================+\n" RESET);

    printf(BRIGHT_WHITE "Query Domain: " RESET BRIGHT_GREEN "%s" RESET "\n", msg->question->q_name);

    const char *type_desc = "";
    switch (msg->question->q_type)
    {
    case DNS_TYPE_A:
        type_desc = "IPv4 Address Record";
        break;
    case DNS_TYPE_AAAA:
        type_desc = "IPv6 Address Record";
        break;
    case DNS_TYPE_CNAME:
        type_desc = "Canonical Name Record";
        break;
    case DNS_TYPE_MX:
        type_desc = "Mail Exchange Record";
        break;
    case DNS_TYPE_TXT:
        type_desc = "Text Record";
        break;
    case DNS_TYPE_NS:
        type_desc = "Name Server Record";
        break;
    case DNS_TYPE_PTR:
        type_desc = "Pointer Record";
        break;
    case DNS_TYPE_SOA:
        type_desc = "Start of Authority Record";
        break;
    default:
        type_desc = "Unknown Record Type";
        break;
    }

    printf(BRIGHT_WHITE "Query Type: " RESET CYAN "%d" RESET " (%s)\n", msg->question->q_type, type_desc);
    printf(BRIGHT_WHITE "Query Class: " RESET CYAN "%d" RESET " (%s)\n",
           msg->question->q_class,
           msg->question->q_class == DNS_CLASS_IN ? "Internet (IN)" : "Unknown Class");
    printf("\n");
}

void print_answer(Dns_Mes *msg)
{
    if (msg->header->ancount == 0)
    {
        printf(YELLOW "+============================================================================+\n");
        printf("|                            NO ANSWER RECORDS                              |\n");
        printf("+============================================================================+\n" RESET);
        printf(DIM "No answer records found in this DNS response.\n" RESET);
        printf("\n");
        return;
    }

    printf(BRIGHT_BLUE "+============================================================================+\n");
    printf("|                              DNS ANSWERS                                   |\n");
    printf("+============================================================================+\n" RESET);

    Dns_rr *current_answer = msg->answer;
    int record_count = 1;

    while (current_answer != NULL)
    {
        // 跳过垃圾数据：Unknown类型(>255)或Unknown类(>255)的记录
        if (current_answer->type > 255 || current_answer->rr_class > 255)
        {
            current_answer = current_answer->next;
            continue;
        }

        printf(BRIGHT_WHITE "Answer Record #%d:\n" RESET, record_count);
        printf(BRIGHT_WHITE "   Name: " RESET BRIGHT_GREEN "%s" RESET "\n", current_answer->name);

        const char *type_desc = "";
        switch (current_answer->type)
        {
        case DNS_TYPE_A:
            type_desc = "IPv4 Address (A)";
            break;
        case DNS_TYPE_AAAA:
            type_desc = "IPv6 Address (AAAA)";
            break;
        case DNS_TYPE_CNAME:
            type_desc = "Canonical Name (CNAME)";
            break;
        case DNS_TYPE_MX:
            type_desc = "Mail Exchange (MX)";
            break;
        case DNS_TYPE_TXT:
            type_desc = "Text Record (TXT)";
            break;
        case DNS_TYPE_NS:
            type_desc = "Name Server (NS)";
            break;
        case DNS_TYPE_PTR:
            type_desc = "Pointer (PTR)";
            break;
        case DNS_TYPE_SOA:
            type_desc = "Start of Authority (SOA)";
            break;
        default:
            type_desc = "Unknown Type";
            break;
        }

        printf(BRIGHT_WHITE "   Type: " RESET CYAN "%d" RESET " (%s)\n", current_answer->type, type_desc);
        printf(BRIGHT_WHITE "   Class: " RESET CYAN "%d" RESET " (%s)\n",
               current_answer->rr_class,
               current_answer->rr_class == DNS_CLASS_IN ? "Internet (IN)" : "Unknown");
        printf(BRIGHT_WHITE "   TTL: " RESET MAGENTA "%d" RESET " seconds (%d minutes)\n",
               current_answer->ttl, current_answer->ttl / 60);
        printf(BRIGHT_WHITE "   Data Length: " RESET CYAN "%d" RESET " bytes\n", current_answer->rd_length);

        printf(BRIGHT_WHITE "   Resource Data: " RESET);

        switch (current_answer->type)
        {
        case DNS_TYPE_A:
            printf(BRIGHT_YELLOW "%d.%d.%d.%d" RESET " (IPv4 Address)\n",
                   current_answer->rd_data.a_record.IP_addr[0],
                   current_answer->rd_data.a_record.IP_addr[1],
                   current_answer->rd_data.a_record.IP_addr[2],
                   current_answer->rd_data.a_record.IP_addr[3]);
            break;

        case DNS_TYPE_AAAA:
            printf(BRIGHT_YELLOW);
            for (int i = 0; i < 16; i += 2)
            {
                printf("%02x%02x", current_answer->rd_data.aaaa_record.IP_addr[i],
                       current_answer->rd_data.aaaa_record.IP_addr[i + 1]);
                if (i < 14)
                    printf(":");
            }
            printf(RESET " (IPv6 Address)\n");
            break;

        case DNS_TYPE_CNAME:
            printf(BRIGHT_YELLOW "%s" RESET " (Canonical Name)\n", current_answer->rd_data.cname_record.cname);
            break;

        case DNS_TYPE_MX:
            printf(BRIGHT_YELLOW "Priority: %d, Exchange: %s" RESET " (Mail Server)\n",
                   current_answer->rd_data.mx_record.preference,
                   current_answer->rd_data.mx_record.exchange);
            break;

        case DNS_TYPE_TXT:
            printf(BRIGHT_YELLOW "\"%s\"" RESET " (Text Data)\n", current_answer->rd_data.txt_record.text);
            break;

        default:
            printf(DIM "Binary data (%d bytes)" RESET "\n", current_answer->rd_length);
            break;
        }

        current_answer = current_answer->next;
        record_count++;
        if (current_answer != NULL)
            printf("\n");
    }
    printf("\n");
}