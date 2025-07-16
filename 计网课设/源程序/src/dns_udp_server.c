#define _BSD_SOURCE
#define _DEFAULT_SOURCE // ��Ҫ������Щ��ʹusleep����
#include "dns_udp_server.h"
#include "config_and_host.h"
#include "domain_trie_map.h"
#include "dns_id_map.h"
#include <unistd.h> // Ϊusleep����
#include <stdlib.h> // Ϊmalloc, free
#include "dns_packet_codec.h"

// ANSI ��ɫ���룬�����ڿ���̨�����ͬ��ɫ���ı���
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define RESET "\x1B[0m"

// �ͻ��˶˿ں͵�ַ���ȱ���
int clientPort;
socklen_t addressLength;
// DNS��������IP��ַ
char *dnsServerAddress = "10.3.9.4";

void initializeSocket()
{
    addressLength = sizeof(struct sockaddr_in);

    // ͳһ��socketϵͳ��ʼ��
    socket_error_t init_result = socket_system_init();
    if (init_result != SOCKET_SUCCESS)
    {
        fprintf(stderr, "Socket system initialization failed: %s\n",
                socket_get_error_string(init_result));
        exit(1);
    }

    // �����ͻ��˺ͷ�����socket
    clientSocket = socket_create(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == SOCKET_INVALID)
    {
        fprintf(stderr, "Error creating client socket: %d\n", socket_get_last_error());
        socket_system_cleanup();
        exit(1);
    }

    serverSocket = socket_create(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == SOCKET_INVALID)
    {
        fprintf(stderr, "Error creating server socket: %d\n", socket_get_last_error());
        socket_close(clientSocket);
        socket_system_cleanup();
        exit(1);
    }

    // ��ʼ����ַ�ṹ
    memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = INADDR_ANY;
    clientAddress.sin_port = htons(DNS_PORT);

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(dnsServerAddress);
    serverAddress.sin_port = htons(DNS_PORT);

    // ���ö˿�����
    socket_error_t reuse_result = socket_set_reuse_addr(clientSocket, true);
    if (reuse_result != SOCKET_SUCCESS)
    {
        fprintf(stderr, "setsockopt(SO_REUSEADDR) failed: %s\n",
                socket_get_error_string(reuse_result));
        socket_close(clientSocket);
        socket_close(serverSocket);
        socket_system_cleanup();
        exit(1);
    }

    // �󶨿ͻ���socket
    socket_error_t bind_result = socket_bind(clientSocket,
                                             (struct sockaddr *)&clientAddress,
                                             sizeof(clientAddress));
    if (bind_result != SOCKET_SUCCESS)
    {
        fprintf(stderr, "Bind failed: %s\n", socket_get_error_string(bind_result));
        socket_close(clientSocket);
        socket_close(serverSocket);
        socket_system_cleanup();
        exit(1);
    }

    // ��ӡ��������Ϣ
    printf("DNS server: %s\n", dnsServerAddress);
    printf("Listening on port %d\n", DNS_PORT);
}

// �ر��׽��ֲ�����socketϵͳ
void closeSocketServer()
{
    socket_close(clientSocket);
    socket_close(serverSocket);
    socket_system_cleanup();
}

// ������ģʽ
void setNonBlockingMode()
{
    // ���Խ��������Ϳͻ����׽�������Ϊ������ģʽ
    socket_error_t server_result = socket_set_nonblocking(serverSocket, true);
    socket_error_t client_result = socket_set_nonblocking(clientSocket, true);

    if (server_result != SOCKET_SUCCESS || client_result != SOCKET_SUCCESS)
    {
        fprintf(stderr, "Failed to set non-blocking mode: %s\n\n",
                socket_get_error_string(server_result != SOCKET_SUCCESS ? server_result : client_result));
        socket_close(serverSocket);
        socket_close(clientSocket);
        socket_system_cleanup();
        exit(EXIT_FAILURE);
    }

    // �ڷ�����ģʽ�£�ѭ���������Կͻ��˺ͷ�����������
    while (1)
    {
        receive_client(); // �������Կͻ��˵�����
        receive_server(); // �������Է�����������

        // ����CPUʹ���ʹ��ߣ����΢С�ӳ�
#ifdef _WIN32
        Sleep(1); // Windows: 1�����ӳ�
#else
        usleep(1000); // Linux: 1�����ӳ�
#endif
    }
}

// ʹ��ͳһpoll�ӿڽ�������ģʽ�µ��¼�����
void setBlockingMode()
{
    // ʹ��ͳһpoll�ӿڽ�������ģʽ�µ��¼�����
    socket_poll_t fds[2];              // ���ڴ洢��Ҫ�������׽��ֺ��¼�����
    fds[0].fd = clientSocket;          // ������Ҫ�������׽���
    fds[0].events = SOCKET_EVENT_READ; // ָ���������¼�����Ϊ "�ɶ�"
    fds[1].fd = serverSocket;
    fds[1].events = SOCKET_EVENT_READ;

    // ѭ���ȴ�ֱ�����¼�����
    while (1)
    {
        // ����ͳһpoll�ӿڵȴ��׽����ϵ��¼���-1 ������ʾ���޵ȴ���ֱ�����¼�������
        socket_error_t poll_result = socket_poll(fds, 2, -1);
        if (poll_result != SOCKET_SUCCESS)
        {
            fprintf(stderr, "Poll failed: %s\n", socket_get_error_string(poll_result));
            socket_close(serverSocket);
            socket_close(clientSocket);
            socket_system_cleanup();
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & SOCKET_EVENT_READ)
        {
            receive_client(); // ����ͻ����¼�
        }
        if (fds[1].revents & SOCKET_EVENT_READ)
        {
            receive_server(); // ����������¼�
        }
    }
}

// �������Կͻ��˵����ݲ����д���
void receive_client()
{
    uint8_t buffer[BUFFER_SIZE];     // ���յı���
    uint8_t buffer_new[BUFFER_SIZE]; // �ظ����ͻ��˵ı���
    Dns_Mes msg;                     // ���Ľṹ��
    uint8_t ip_addr[4] = {0};        // ��ѯ�����õ���IP��ַ
    int msg_size = -1;               // ���Ĵ�С
    int is_found = 0;                // �Ƿ�鵽

    msg_size = socket_recv_from(clientSocket, buffer, sizeof(buffer), (struct sockaddr *)&clientAddress, &addressLength);

    // ֻ���������յ�����ʱ�Ŵ���ʹ�ӡ������Ϣ
    if (msg_size > 0)
    {
        if (debug_mode == 1)
            printf(RED "�յ����Կͻ��˵�DNS��ѯ����\n\n" RESET);

        uint8_t *start = buffer;
        /* �����ͻ��˷�����DNS���ģ����䱣�浽msg�ṹ���� */
        string_to_dnsstruct(&msg, buffer, start);

        /* ��cache������� - ֻ����A��¼(IPv4) */
        if (msg.question->q_type == DNS_TYPE_A)
        {
            is_found = cache_query(ip_addr, msg.question->q_name, DNS_TYPE_A);
        }
        else
        {
            is_found = 0; // ��A��¼��ʹ�û��棬ֱ��ת��
            if (debug_mode == 1)
                printf(RED "Non-A record query (Type: %d), skip cache lookup\n\n" RESET, msg.question->q_type);
        }

        /* ��cacheδ�鵽�����host�ļ����� */
        if (is_found == 0)
        {
            if (debug_mode == 1)
                printf(RED "��cache��û���ҵ��������,�����ڱ���hosts�ļ������.\n\n" RESET);

            // ֻ��A��¼��ѯ������hosts�ļ�
            if (msg.question->q_type == DNS_TYPE_A)
            {
                is_found = query_node(msg.question->q_name, ip_addr);
            }
            else
            {
                is_found = 0; // ��A��¼����ѯhosts�ļ�
                if (debug_mode == 1)
                    printf(RED "Non-A record query (Type: %d), skip hosts file lookup\n\n" RESET, msg.question->q_type);
            }

            /* ��δ�鵽�����Ͻ�Զ��DNS����������*/
            if (is_found == 0)
            {
                /* ����Ҫ����Զ��DNS�������İ�������ID */
                uint16_t newID = reset_id(msg.header->id, clientAddress);
                if (newID == UINT16_MAX)
                {
                    printf(RED "Error: ID list is full, cannot forward query.\n\n" RESET);
                }
                else
                {
                    // �޸ı����е�IDΪ�·����ID (�����ֽ���)
                    uint16_t network_newID = htons(newID);
                    memcpy(buffer, &network_newID, sizeof(uint16_t));

                    socket_send_to(serverSocket, buffer, msg_size, (struct sockaddr *)&serverAddress, addressLength);
                    if (debug_mode == 1)
                        printf(RED "����hosts�ļ���û���ҵ��������,�����µ�ID %u,�����ϼ�DNS������.\n\n" RESET, newID);
                }
                return;
            }
        }

        uint8_t *end;
        end = dnsstruct_to_string(&msg, buffer_new, ip_addr);

        int len = end - buffer_new;

        /* ��DNSӦ���ķ��ؿͻ��� */
        socket_send_to(clientSocket, buffer_new, len, (struct sockaddr *)&clientAddress, addressLength);

       
    }
}

// ����DNS��Ӧ�е�A��¼�����뻺��
void parse_and_cache_dns_response(Dns_Mes *msg, uint8_t *buffer)
{
    (void)buffer; // ��ֹδʹ�ò�������
    
    if (!msg || !msg->answer || !msg->question)
    {
        return;
    }

    // ֻ����A��¼��Ӧ
    if (msg->question->q_type != DNS_TYPE_A)
    {
        if (debug_mode == 1)
            printf(RED "Non-A record response (Type: %d), skip caching\n\n" RESET, msg->question->q_type);
        return;
    }

    // ����Ƿ��гɹ���A��¼��
    if (msg->header->ancount > 0 && msg->header->rcode == DNS_RCODE_OK)
    {
        Dns_rr *answer = msg->answer;

        while (answer)
        {
            // �ҵ�A��¼
            if (answer->type == DNS_TYPE_A && answer->rd_length == 4)
            {
                uint8_t ipv4[4];
                memcpy(ipv4, answer->rd_data.a_record.IP_addr, 4);

                // ���뻺��
                insert_cache(ipv4, msg->question->q_name, DNS_TYPE_A);

                if (debug_mode == 1)
                    printf(RED "Remote DNS response cached! Domain: %s, IP: %d.%d.%d.%d\n\n" RESET,
                           msg->question->q_name, ipv4[0], ipv4[1], ipv4[2], ipv4[3]);

                break; // ֻ�����һ��A��¼
            }
            answer = answer->next;
        }
    }
    else
    {
        if (debug_mode == 1)
            printf(RED "DNS response not successful (rcode: %d, ancount: %d), not caching\n\n" RESET,
                   msg->header->rcode, msg->header->ancount);
    }
}

// ��������Զ��DNS����������Ӧ��ת�����ͻ���
void receive_server()
{
    uint8_t buffer[BUFFER_SIZE]; // ���յı���
    Dns_Mes msg;
    int msg_size = -1; // ���Ĵ�С

    /* ���Խ���Զ��DNS������������DNSӦ���� */
    msg_size = socket_recv_from(serverSocket, buffer, sizeof(buffer), (struct sockaddr *)&serverAddress, &addressLength);

    /* ������յ���DNSӦ���� */
    if (msg_size > 0)
    {
        if (debug_mode == 1)
            printf(RED "�յ��ϼ�DNS������������DNSӦ����\n\n" RESET);

        string_to_dnsstruct(&msg, buffer, buffer);

        /* IDת���Ͱ�ȫ��� */
        uint16_t response_id = msg.header->id;
        uint16_t original_user_id;
        struct sockaddr_in original_client_addr;

        // ʹ���µİ�ȫ������ȡӳ����Ϣ
        if (get_id_mapping(response_id, &original_user_id, &original_client_addr))
        {
            // ����������DNS��Ӧ�е�A��¼
            parse_and_cache_dns_response(&msg, buffer);

            // ����ӦID�Ļ�ԭʼ�û�ID
            uint16_t network_user_id = htons(original_user_id);
            memcpy(buffer, &network_user_id, sizeof(uint16_t));

            // �ͷ�IDӳ��
            release_id_mapping(response_id);

            // ���͵�ԭʼ�ͻ��˵�ַ
            socket_send_to(clientSocket, buffer, msg_size, (struct sockaddr *)&original_client_addr, addressLength);

            if (debug_mode == 1)
                printf(GREEN "Response forwarded to client. ID %u -> %u\n\n" RESET, response_id, original_user_id);

            if (log_mode == 1)
            {
                write_log(msg.question->q_name, NULL);
            }
        }
        else
        {
            if (debug_mode == 1)
                fprintf(stderr, RED "Error: Invalid or expired ID mapping for response ID %u\n\n" RESET, response_id);
        }
    }
}