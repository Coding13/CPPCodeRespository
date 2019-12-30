#include "stdafx.h"
#include"HttpInterfaces.h"
#include <iostream>
#include "LogWriter.h"
#include <memory>
#include "spdlog/fmt/bin_to_hex.h"
#ifdef _DEBUG
#pragma comment(lib,"Lib\\libeay32MDd.lib")
#pragma comment(lib,"Lib\\ssleay32MDd.lib")
#else
#pragma comment(lib,"Lib\\libeay32MD.lib")
#pragma comment(lib,"Lib\\ssleay32MD.lib")
#endif
using namespace std;
static struct mg_mgr m_mgr;//事件管理器
static struct mg_bind_opts m_bind_opts;//用于绑定的属性参数
void __stdcall Init()
{
	LogInfo("Init ...");
}
int __stdcall Bind(mg_event_handler_t callback, const char * port, const char* certName, const char* keyName, char ** errormsg)
{
	if (port == NULL || strcmp(port, "") == 0)
	{
		LogInfo("HttpInit:{}", "parameters Invalid");
		strncpy(*errormsg, "parameters Invalid", strlen("parameters Invalid"));
		return -1;
	}
	mg_mgr_init(&m_mgr, NULL);
	LogInfo("Bind port:{},cert:{},key:{}", port, certName == NULL ? "" : certName, keyName == NULL ? "" : keyName);

	if (certName != NULL && strcmp(certName, "") != 0)
	{
		m_bind_opts.ssl_cert = certName;
	}
	if (keyName != NULL && strcmp(keyName, "") != 0)
	{
		m_bind_opts.ssl_key = keyName;

	}
	m_bind_opts.error_string = NULL;

	mg_connection *cn = mg_bind_opt(&m_mgr, port, callback, m_bind_opts);
	if (cn == NULL)
	{
		errormsg = (char **)m_bind_opts.error_string;
		LogInfo("Failed to create listener");
		return -1;
	}
	LogInfo("Bind Success");
	mg_set_protocol_http_websocket(cn);
	LogInfo("connection Success");
	return 0;
}

void __stdcall Close()
{
	LogInfo("Close Serveice...");
	mg_mgr_free(&m_mgr);
	return;
}
void __stdcall SendErrorResponse(mg_connection *connection, int errCode, const char * errMsg)
{
	if (connection == NULL)
	{
		return;
	}
	int len = strlen(errMsg);
	char * resp = new char[len + 1];
	strncpy(resp, errMsg, len);
	resp[len] = '\0';

	mg_send_head(connection, errCode, strlen(resp), "Access-Control-Allow-Origin: *\r\nContent-Type:text/plain;charset=GBK");
	mg_send(connection, resp, strlen(resp));
	LogInfo("SendData: {},errcode:{}", resp, errCode);
	delete[] resp;
	return;
}
void __stdcall SendResponse(mg_connection *connection, const char * respMsg)
{
	if (!connection || strlen(respMsg) <= 0)
	{
		mg_http_send_error(connection, 400, "Error : bad request");
		return;
	}
	int len = strlen(respMsg);
	char * resp = new char[len + 1];
	strncpy(resp, respMsg, len);
	resp[len] = '\0';

	mg_send_head(connection, 200, strlen(resp), "Access-Control-Allow-Origin: *\r\nContent-Type:text/plain;charset=GBK");//"Access-Control-Allow-Origin: *"
	mg_send(connection, resp, strlen(resp));
	LogInfo("SendData: {}", resp);
	delete[] resp;
}
//通过调用循环创建一个事件mg_mgr_poll()循环
void __stdcall StartListen(int timeOut)
{
	mg_mgr_poll(&m_mgr, timeOut);
}