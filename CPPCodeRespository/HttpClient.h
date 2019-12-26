#pragma once
#ifndef _HTTPCLIENT_INCLUDE_
#define _HTTPCLIENT_INCLUDE_
#include <string>
#include <iostream>
#include <memory>
#define MG_ENABLE_SSL 1
#include "mongoose.h"
#include <functional>

// 此处必须用function类，typedef再后面函数指针赋值无效
using ReqCallback = std::function<void(std::string)>;

class HttpClient
{
public:
	HttpClient() {}
	~HttpClient() {}

	static void SendReq(const std::string &url,  ReqCallback req_callback,const std::string &headers=nullptr, const std::string &post_data=nullptr);
	static void OnHttpEvent(mg_connection *connection, int event_type, void *event_data);
	static int s_exit_flag;
	static ReqCallback s_req_callback;
};
#endif