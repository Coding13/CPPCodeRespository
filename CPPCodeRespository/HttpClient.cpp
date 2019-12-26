#include "HttpClient.h"
#include "LogWriter.h"

// 初始化client静态变量
int HttpClient::s_exit_flag = 0;
ReqCallback HttpClient::s_req_callback;

auto m_clientlogger = spdlog::daily_logger_mt("HttpClient", "logs/HttpClient.log", 0, 0);
// 客户端的网络请求响应
void HttpClient::OnHttpEvent(mg_connection *connection, int event_type, void *event_data)
{
	http_message *hm = (struct http_message *)event_data;
	int connect_status;

	switch (event_type)
	{
	case MG_EV_CONNECT:
		connect_status = *(int *)event_data;
		if (connect_status != 0)
		{
			m_clientlogger->info("Error connecting to server, error code:{}", connect_status);
			s_exit_flag = 1;
		}
		break;
	case MG_EV_HTTP_REPLY:
	{
		m_clientlogger->info("Got reply:{}{}", (int)hm->body.len, hm->body.p);
		std::string rsp = std::string(hm->body.p, hm->body.len);
		connection->flags |= MG_F_SEND_AND_CLOSE;
		s_exit_flag = 1; // 每次收到请求后关闭本次连接，重置标记

						 // 回调处理
		s_req_callback(rsp);
	}
	break;
	case MG_EV_CLOSE:
		if (s_exit_flag == 0)
		{
			m_clientlogger->info("Server closed connection");
			s_exit_flag = 1;
		};
		break;
	default:
		break;
	}
}


// 发送一次请求，并回调处理，然后关闭本次连接,不填写post_data参数的时候默认使用get请求，填写后使用post请求
void HttpClient::SendReq(const std::string &url, ReqCallback req_callback, const std::string &headers, const std::string &post_data)
{
	s_exit_flag = 0;
	// 给回调函数赋值
	s_req_callback = req_callback;
	mg_mgr mgr;
	mg_mgr_init(&mgr, NULL);
	auto connection = mg_connect_http(&mgr, OnHttpEvent, url.c_str(), headers.c_str(), post_data.c_str());
	mg_set_protocol_http_websocket(connection);

	m_clientlogger->info("Send http request {}", url.c_str());

	while (s_exit_flag == 0)
		mg_mgr_poll(&mgr, 1000);

	mg_mgr_free(&mgr);
}