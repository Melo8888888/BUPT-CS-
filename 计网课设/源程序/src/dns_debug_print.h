/*
���ļ����ڶ��� ��ӡ DNS ���ģ��ֽ������ṹ�壩�Ľӿ�
*/

#pragma once

#include "dns_protocol_structs.h"

// ��ӡ DNS �����ֽ���
void print_dstring(char *pstring, unsigned int length);

// ��ӡheader
void print_header(Dns_Mes *msg);

// ��ӡquestion
void print_question(Dns_Mes *msg);

// ��ӡanswer��RR��
void print_answer(Dns_Mes *msg);
