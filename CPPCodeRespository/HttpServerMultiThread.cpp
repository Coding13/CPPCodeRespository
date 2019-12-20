#include"HttpServerMultiThread.h"
#ifdef  _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif 

#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

static sig_atomic_t s_received_signal = 0;
static const char *s_http_port = "8888";
static const int s_num_worker_threads = 5;
static unsigned long s_next_id = 0;

static void signal_handler(int sig_num) 
{
	signal(sig_num, signal_handler);
	s_received_signal = sig_num;
}
static struct mg_serve_http_opts s_http_server_opts;
static sock_t sock[2];

static void on_work_complete(struct mg_connection *nc, int ev, void *ev_data) 
{
	(void)ev;
	char s[32];
	struct mg_connection *c;
	for (c = mg_next(nc->mgr, NULL); c != NULL; c = mg_next(nc->mgr, c)) 
	{
		if (c->user_data != NULL) 
		{
			struct work_result *res = (struct work_result *)ev_data;
			if ((unsigned long)c->user_data == res->conn_id)
			{
				sprintf(s, "conn_id:%lu sleep:%d", res->conn_id, res->sleep_time);
				mg_send_head(c, 200, strlen(s), "Content-Type: text/plain");
				mg_printf(c, "%s", s);
			}
		}
	}
}
  
void *worker_thread_proc(void *param) 
{
	struct mg_mgr *mgr = (struct mg_mgr *) param;
	while (s_received_signal == 0) {
		struct work_request req = { 0 };
		
		memset(&req, 0, sizeof(req));
		union socket_address sa;
		memset(&sa, 0, sizeof(sa));
		socklen_t sa_len = sizeof(sa);

		char recvBuf[4096];
		memset(recvBuf, 0, 4096);
		//if (recv(sock[1], revData,1024,0) == SOCKET_ERROR)//windowsƽ̨��ʹ��
		//if (read(sock[1], &req, sizeof(req)) < 0)//��sockר�����ڶ� linuxƽ̨ʹ��
#ifdef _WIN32
		if (recvfrom(sock[1], recvBuf, 4096, 0,&(sa.sa),(int *) sizeof(sa.sa) )< 0)
		{
			perror("Reading worker sock");
		}
#else
		if (read(sock[1], &req, sizeof(req)) < 0)//��sockר�����ڶ� linuxƽ̨ʹ��
			perror("Reading worker sock");
#endif
		memcpy(&req, recvBuf, sizeof(work_request));

		//��ӡ���յ�����Ϣ
		cout << req.conn_id << endl;
		string strUri = req.message.uri.p;
		strUri = strUri.substr(0, req.message.uri.len);
		cout << strUri << endl;
		string strMethod = req.message.method.p;
		strMethod = strMethod.substr(0, req.message.method.len);
		cout << strMethod << endl;
		
		int r = rand() % 10;
		sleep(r);
		struct work_result res = { req.conn_id, r };
		mg_broadcast(mgr, on_work_complete, (void *)&res, sizeof(res));
	}
	return NULL;
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) 
{
	(void)nc;
	(void)ev_data;

	switch (ev)
	{
	case MG_EV_ACCEPT:
		nc->user_data = (void *)++s_next_id;
		break;
	case MG_EV_HTTP_REQUEST: 
	{
		struct work_request req = { (unsigned long)nc->user_data };
		req.message = *(http_message* )ev_data;
		char sendBuf[4096];
		memset(sendBuf, 0, 4096);
		memcpy(sendBuf, &req, sizeof(work_request));

		union socket_address sa;
		memset(&sa, 0, sizeof(sa));
		socklen_t sa_len = sizeof(sa);
		//if (send(sock[0], sendData, 1024, 0) == SOCKET_ERROR)
		//if (write(sock[0], &req, sizeof(req)) < 0)//��sockר������д
		//�������̷߳���Ϣ �����߳̽��о������Ϣ����
#ifdef _WIN32
		if (sendto(sock[0], sendBuf, sizeof(sendBuf), 0, &(sa.sa), sizeof(sa.sa)) < 0)
		{
			perror("Writing worker sock");
		}
#else
		if (write(sock[0], &req, sizeof(req)) < 0)//��sockר������д
			perror("Writing worker sock");
#endif
		break;
	}
	case MG_EV_CLOSE: 
	{
		if (nc->user_data) nc->user_data = NULL;
	}
	}
}

int MultiThreadStart()
{
	struct mg_mgr mgr;
	struct mg_connection *nc;

	//windowsƽ̨ʹ�õ�ʱ�� ����ͨ��WSAStartup������ɶ�Winsock����ĳ�ʼ��
#ifdef _WIN32
	WSADATA wsaDate;
	int nRet = WSAStartup(MAKEWORD(2, 1), &wsaDate);
	if (nRet != NO_ERROR)
	{
		perror("WSAStartup error");
		return -1;
	}
#endif

	//���ڽ���ͨ��
	if (mg_socketpair(sock, SOCK_STREAM) == 0) {
		perror("Opening socket pair");
		return -1;
	}

	//�����ź� ������һЩ����Ĵ��� 
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	mg_mgr_init(&mgr, NULL);

	//�󶨶˿�
	nc = mg_bind(&mgr, s_http_port, ev_handler);
	if (nc == NULL) {
		printf("Failed to create listener\n");
		return 1;
	}

	//��������
	mg_set_protocol_http_websocket(nc);
	s_http_server_opts.document_root = ".";  // Serve current directory
	s_http_server_opts.enable_directory_listing = "no";

	//���������߳� ����Ĵ�����Ϣ
	for (int i = 0; i < s_num_worker_threads; i++)
	{
		mg_start_thread(worker_thread_proc, &mgr);
	}

	printf("Started on port %s\n", s_http_port);
	while (s_received_signal == 0)
	{
		mg_mgr_poll(&mgr, 200);
	}

	mg_mgr_free(&mgr);

	closesocket(sock[0]);
	closesocket(sock[1]);

#ifdef _WIN32
	//�����Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ
	WSACleanup();
#endif 

	
	return 0;
}