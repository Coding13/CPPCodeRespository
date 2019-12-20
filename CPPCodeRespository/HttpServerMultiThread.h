#pragma once
#ifndef _HTTPSERVER_MULTOTHREAD_INCLUDE_ 
#define _HTTPSERVER_MULTOTHREAD_INCLUDE_
static void signal_handler(int sig_num);

#define MG_ENABLE_SSL 1 //����https
#define MG_ENABLE_THREADS 1 //���ö��߳�
#include "mongoose.h"

//IO�߳��յ�����󣬽�����ͨ��socket���������߳�
//�����̴߳������,ͨ��mg_broadcast��������IO�߳�
//mongoose�ڲ�Ҳ��һ��socket���ڹ����߳���IO�߳�ͨ�ţ�IO�߳��յ������Ӧ�����ӻ�д���

//IO�߳��յ������ͨ��sock������Ϣ�������߳�
struct work_request
{
	unsigned long conn_id;//��ID�����ж����ĸ����ӵ���Ϣ
	http_message message; //���Ը����Լ���Ҫ�������Ϣ������������
};

//��ָ����Connection�ش���Ϣ
struct work_result
{
	unsigned long conn_id;
	int sleep_time;
};

//�������֮����лش�����
static void on_work_complete(struct mg_connection *nc, int ev, void *ev_data);

//�����߳�
void *worker_thread_proc(void *param);

//ֻ������Ϣ�ַ� ��ֱ�Ӵ���
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data);

int MultiThreadStart();

#endif // ! 

