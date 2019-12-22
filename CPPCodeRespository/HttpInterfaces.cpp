#include "stdafx.h"
#include"HttpInterfaces.h"
#include <iostream>
#include "LogWriter.h"
#include <memory>

#ifdef _DEBUG
#pragma comment(lib,"Lib\\libeay32MDd.lib")
#pragma comment(lib,"Lib\\ssleay32MDd.lib")
#else
#pragma comment(lib,"Lib\\libeay32MD.lib")
#pragma comment(lib,"Lib\\ssleay32MD.lib")
#endif
static struct mg_mgr m_mgr;//事件管理器
static struct mg_bind_opts m_bind_opts;//用于绑定的属性参数
static char * m_port = NULL;//端口

auto m_loggerInterface = daily_logger_mt("HttpInterfacesForNet", "logs/Http.log", 0, 0);
void __stdcall Init()
{
	m_loggerInterface->flush_on(level::info);
	m_loggerInterface->info("Init ...");
}
int __stdcall Bind(mg_event_handler_t callback, const char * port, const char* certName, const char* keyName, char ** errormsg)
{
	if (port == NULL || strcmp(port, "") == 0)
	{
		m_loggerInterface->info("HttpInit:{}", "parameters Invalid");
		strncpy(*errormsg, "parameters Invalid", strlen("parameters Invalid"));
		return -1;
	}
	mg_mgr_init(&m_mgr, NULL);
	m_loggerInterface->info("Bind port:{},cert:{},key:{}", port, certName == NULL ? "" : certName, keyName == NULL ? "" : keyName);

	int len = strlen(port);
	m_port = new char[len + 1];
	strncpy(m_port, port, len + 1);
	if (certName != NULL && strcmp(certName, "") != 0)
	{
		memset(&m_bind_opts, 0, sizeof(m_bind_opts));
		len = strlen(certName);
		m_bind_opts.ssl_cert = new char[len + 1];
		strncpy((char *)m_bind_opts.ssl_cert, certName, len + 1);
	}
	if (keyName != NULL && strcmp(keyName, "") != 0)
	{
		len = strlen(keyName);
		m_bind_opts.ssl_key = new char[len + 1];
		strncpy((char *)m_bind_opts.ssl_key, keyName, len + 1);
	}
	m_bind_opts.error_string = NULL;

	//m_loggerInterface->info("Https port:{},cert:{},key:{}", m_port, m_bind_opts.ssl_cert, m_bind_opts.ssl_key);
	mg_connection *cn = mg_bind_opt(&m_mgr, m_port, callback, m_bind_opts);
	if (cn == NULL)
	{
		errormsg = (char **)m_bind_opts.error_string;
		m_loggerInterface->info("Failed to create listener");
		return -1;
	}
	m_loggerInterface->info("Bind Success");
	mg_set_protocol_http_websocket(cn);
	m_loggerInterface->info("connection Success");
	return 0;
}

void __stdcall Close()
{
	mg_mgr_free(&m_mgr);
	if (m_port != NULL)
		delete m_port;
	return;
}
void __stdcall SendErrorResponse(mg_connection *connection, int errCode, const char * errMsg)
{
	if (connection == NULL)
	{
		return;
	}
	m_loggerInterface->info("errCode:{},errMsg:{}", errCode, errMsg);
	int len = strlen(errMsg);
	char* resp = new char[len + 1];
	strncpy(resp, errMsg, len + 1);
	resp[len] = '\0';

	mg_send_head(connection, 200, (strlen(resp) + 1), "Access-Control-Allow-Origin: *\r\nContent-Type:text/plain;charset=utf-8\r\nConnection: close");//"Access-Control-Allow-Origin: *" \r\nConnection: close
	mg_send(connection, resp, strlen(resp) + 1);
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
	strncpy(resp, respMsg, len + 1);
	resp[len] = '\0';

	mg_send_head(connection, 200, (strlen(resp) + 1), "Access-Control-Allow-Origin: *\r\nContent-Type:text/plain;charset=utf-8");//"Access-Control-Allow-Origin: *"
	mg_send(connection, resp, strlen(resp) + 1);
	m_loggerInterface->info("SendData: {}", resp);
	delete[] resp;
}
//通过调用循环创建一个事件mg_mgr_poll()循环
void __stdcall StartListen(int timeOut)
{
	mg_mgr_poll(&m_mgr, timeOut);
}