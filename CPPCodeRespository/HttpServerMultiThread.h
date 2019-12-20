#pragma once
#ifndef _HTTPSERVER_MULTOTHREAD_INCLUDE_ 
#define _HTTPSERVER_MULTOTHREAD_INCLUDE_
static void signal_handler(int sig_num);

#define MG_ENABLE_SSL 1 //启用https
#define MG_ENABLE_THREADS 1 //启用多线程
#include "mongoose.h"

//IO线程收到请求后，将请求通过socket发往工作线程
//工作线程处理完后,通过mg_broadcast方法返回IO线程
//mongoose内部也有一对socket用于工作线程向IO线程通信，IO线程收到后向对应的连接回写结果

//IO线程收到请求后，通过sock发送消息给工作线程
struct work_request
{
	unsigned long conn_id;//该ID用于判断是哪个连接的消息
	http_message message; //可以根据自己需要处理的信息在这里继续添加
};

//向指定的Connection回传消息
struct work_result
{
	unsigned long conn_id;
	int sleep_time;
};

//处理完成之后进行回传处理
static void on_work_complete(struct mg_connection *nc, int ev, void *ev_data);

//工作线程
void *worker_thread_proc(void *param);

//只进行消息分发 不直接处理
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data);

int MultiThreadStart();

#endif // ! 

