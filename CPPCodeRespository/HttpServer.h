#ifndef _HTTPSSERERBYMONGOOSE_INCLUDE_
#define _HTTPSSERERBYMONGOOSE_INCLUDE_

#include <string>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <functional>
#include <utility>
#define MG_ENABLE_SSL 1
#include "mongoose.h"
using namespace std;
// ����http����callback
typedef void OnResponseCallback(mg_connection *c, std::string);

// ����http����handler
using ReqHandler = std::function<bool(std::string, std::string, mg_connection *c, OnResponseCallback)>;

class HttpServer
{
public:
	//��ʼ������������
	void init(const std::string port);
	//����Https����
	void startHttpService(std::string certName,std::string keyName);
	//����Http����
	void startHttpService();
	//ֹͣ����
	static bool Close(mg_mgr mgr);
private:
	//ע���¼�������
	void AddHandler(const std::string &url, ReqHandler req_handler); 
	//�Ƴ��¼�������
	void RemoveHandler(const std::string &url); 
	//·���ж� uri����
	static bool route_check(http_message *http_msg, const char *route_prefix);
	static void ev_handler(struct mg_connection *nc, int ev, void *p);
	//��������
	static void SendData(mg_connection * nc,const char* sdata);
	//��������
	static void SendResponse(mg_connection * nc,const char * rsp);
private:
	std::string m_port;
	static mg_mgr mgr;
	//�ص�����ӳ���
	static std::unordered_map<std::string, ReqHandler> s_handler_map;
};
#endif//_HTTPSSERERBYMONGOOSE_INCLUDE_
