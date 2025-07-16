#include "dns_id_map.h"
/*
 * DNS IDӳ��ģ�� (dns_id_map.c)
 * =================================
 *
 * ����˵��:
 * ��ģ��ΪDNS�м̷������ṩ�ͻ���IDӳ�����������ͻ���ͬʱ����DNS��ѯʱ��
 * ��ҪΪÿ����ѯ����Ψһ���ڲ�ID����ά����ԭʼ�ͻ���ID��ӳ���ϵ��ȷ����Ӧ
 * �ܹ���ȷ·�ɻض�Ӧ�Ŀͻ��ˡ�
 *
 * ʹ������:
 * 1. �ͻ��˲�ѯ -> reset_id()�����ڲ�ID
 * 2. ת��������DNS������(ʹ���ڲ�ID)
 * 3. �յ���Ӧ -> get_id_mapping()��ȡԭʼ�ͻ���
 * 4. ��Ӧ·�ɻؿͻ��� -> release_id_mapping()�ͷ���Դ
 *
 * ����:
 * ��ģ��ʵ����DNS�м̷������е�IDӳ�������ơ���DNS�м̳����£��ͻ��˷���DNS��ѯ
 * ���м̷��������м̷�������Ҫ����ѯת��������DNS�����������ڿ��ܴ��ڶ���ͻ���ͬʱ
 * ��ѯ�������ǵ�DNS��ѯID���ܳ�ͻ�������Ҫһ��IDӳ���������
 * 1. Ϊÿ���ͻ��˲�ѯ����Ψһ���ڲ�ID
 * 2. ά���ͻ���ԭʼID���ڲ�ID��ӳ���ϵ
 * 3. ���յ����η�������Ӧʱ���ܹ���ȷ·�ɻ�ԭʼ�ͻ���
 *
 *
 * ���ݽṹ:
 * - ClientSession���飺�̶���С��IDӳ���(MAX_ID_SIZE=128)
 * - ÿ����Ŀ�������û�ԭʼID���ͻ��˵�ַ������ʱ��
 * - ʹ������������Ϊ�ڲ������ID���򻯲����߼�
 *
 * ������ȫ:
 * - id_map_mutex��ȫ�ֻ�������������ID����
 * - ���й���������ʹ��MUTEX_LOCK/MUTEX_UNLOCK��װ
 * - ֧�ֶ��߳�
 *
 * ID�������:
 * 1. ���ȷ�����ȫ���еĲ�λ(user_id=0, expire_time=0)
 * 2. ���޿��в�λ��������ѹ��ڵĲ�λ���и���
 * 3. �����޿��ò�λ������UINT16_MAX��ʾ����ʧ��
 *
 * ���ڻ���:
 * - ÿ��IDӳ�䶼��expire_timeʱ���
 * - Ĭ�Ϲ���ʱ��ΪID_EXPIRE_TIME��(4��)
 * - ���ڵ�ӳ����Ա���������
 * - ֧�������ͷź��Զ��������ֻ��շ�ʽ
 *
 * ʹ��ʾ��:
 * 1. �ͻ���A(192.168.1.100)���Ͳ�ѯID=1234���м̷�����
 * 2. �м̷���������reset_id(1234, client_addr)�����ڲ�ID=5
 * 3. �м̷�������ID=5������DNS���������Ͳ�ѯ
 * 4. ���η������ظ�ID=5����Ӧ
 * 5. �м̷���������get_id_mapping(5)��ȡԭʼ�ͻ�����Ϣ
 * 6. �м̷���������Ӧ��ID�Ļ�1234�����͸��ͻ���A
 * 7. ����release_id_mapping(5)�ͷ�ӳ����Դ
 *
 * ��������:
 * - ʱ�临�Ӷȣ�O(MAX_ID_SIZE) = O(128) ����O(1)
 * - �ռ临�Ӷȣ�O(MAX_ID_SIZE) = �̶��ڴ�ռ��
 * - �������ܣ��ܻ�����������֧�ֶ��̰߳�ȫ����
 
 * ��������:
 * - �̰߳�ȫ��ID����͹���
 * - �Զ����ڻ��ջ���(4�볬ʱ)
 * - O(1)ʱ�临�ӶȵĲ�������
 * - 128������ID֧��
 *
 * ��Ҫ����:
 * - reset_id(): Ϊ�ͻ��˲�ѯ�����µ��ڲ�ID
 * - get_id_mapping(): �����ڲ�ID����ԭʼ�ͻ�����Ϣ
 * - release_id_mapping(): �����ͷ�IDӳ��
 * - print_id_stats(): ��ӡͳ����Ϣ
 */







// ȫ�ֻ�����
// id_map_mutex������dnsrelay_main.c�ж���

uint16_t reset_id(uint16_t user_id, struct sockaddr_in client_address)
{
    MUTEX_LOCK(&id_map_mutex);

    time_t current_time = time(NULL);
    uint16_t allocated_id = UINT16_MAX;

    // ���Ȳ�����ȫ���еĲ�λ����δʹ�ù��ģ�
    for (uint16_t i = 0; i < MAX_ID_SIZE; i++)
    {
        if (ID_list[i].user_id == 0 && ID_list[i].expire_time == 0)
        {
            ID_list[i].user_id = user_id;
            ID_list[i].client_address = client_address;
            ID_list[i].expire_time = current_time + ID_EXPIRE_TIME;
            allocated_id = i;
            break;
        }
    }

    // ���û���ҵ���ȫ���еĲ�λ�����ҹ��ڵĲ�λ
    if (allocated_id == UINT16_MAX)
    {
        for (uint16_t i = 0; i < MAX_ID_SIZE; i++)
        {
            if (ID_list[i].expire_time < current_time)
            {
                // ������ڲ�λ������
                ID_list[i].user_id = user_id;
                ID_list[i].client_address = client_address;
                ID_list[i].expire_time = current_time + ID_EXPIRE_TIME;
                allocated_id = i;
                break;
            }
        }
    }

    MUTEX_UNLOCK(&id_map_mutex);
    return allocated_id;
}

void init_ID_list()
{
    MUTEX_INIT(&id_map_mutex);

    for (int i = 0; i < MAX_ID_SIZE; i++)
    {
        ID_list[i].user_id = 0;
        ID_list[i].expire_time = 0;
        memset(&(ID_list[i].client_address), 0, sizeof(struct sockaddr_in));
    }
}
//ӳ���ȥ �յ��ϼ��Ļظ� ��  �ͻ���
int get_id_mapping(uint16_t id, uint16_t *user_id, struct sockaddr_in *client_addr)
{
    if (id >= MAX_ID_SIZE || user_id == NULL || client_addr == NULL)
    {
        return 0; // ��Ч����
    }

    MUTEX_LOCK(&id_map_mutex);

    time_t current_time = time(NULL);
    int result = 0;

    // ���ID�Ƿ���Ч��δ����
    if (ID_list[id].expire_time >= current_time && ID_list[id].user_id != 0)
    {
        *user_id = ID_list[id].user_id;
        *client_addr = ID_list[id].client_address;
        result = 1; // �ɹ�
    }

    MUTEX_UNLOCK(&id_map_mutex);
    return result;
}

int release_id_mapping(uint16_t id)
{
    if (id >= MAX_ID_SIZE)
    {
        return 0; // ��ЧID
    }

    MUTEX_LOCK(&id_map_mutex);

    // ���IDӳ��
    ID_list[id].user_id = 0;
    ID_list[id].expire_time = 0;
    memset(&(ID_list[id].client_address), 0, sizeof(struct sockaddr_in));

    MUTEX_UNLOCK(&id_map_mutex);
    return 1; // �ɹ�
}

void print_id_stats()
{
    MUTEX_LOCK(&id_map_mutex);

    time_t current_time = time(NULL);
    int active_count = 0;
    int expired_count = 0;

    for (int i = 0; i < MAX_ID_SIZE; i++)
    {
        if (ID_list[i].user_id != 0)
        {
            if (ID_list[i].expire_time >= current_time)
            {
                active_count++;
            }
            else
            {
                expired_count++;
            }
        }
    }

    printf("ID Mapping Stats: Active=%d, Expired=%d, Total=%d\n",
           active_count, expired_count, MAX_ID_SIZE);

    MUTEX_UNLOCK(&id_map_mutex);
}
