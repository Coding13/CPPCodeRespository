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
// 定义http返回callback
typedef void OnResponseCallback(mg_connection *c, std::string);

// 定义http请求handler
using ReqHandler = std::function<bool(std::string, std::string, mg_connection *c, OnResponseCallback)>;

class HttpServer
{
public:
	//初始化服务器参数
	void init(const std::string port);
	//启动Https服务
	void startHttpService(std::string certName,std::string keyName);
	//启动Http服务
	void startHttpService();
	//停止服务
	static bool Close(mg_mgr mgr);
private:
	//注册事件处理函数
	void AddHandler(const std::string &url, ReqHandler req_handler); 
	//移除事件处理函数
	void RemoveHandler(const std::string &url); 
	//路径判断 uri解析
	static bool route_check(http_message *http_msg, const char *route_prefix);
	static void ev_handler(struct mg_connection *nc, int ev, void *p);
	//发送数据
	static void SendData(mg_connection * nc,const char* sdata);
	//发送数据
	static void SendResponse(mg_connection * nc,const char * rsp);
private:
	std::string m_port;
	static mg_mgr mgr;
	//回调函数映射表
	static std::unordered_map<std::string, ReqHandler> s_handler_map;
};
#endif//_HTTPSSERERBYMONGOOSE_INCLUDE_
