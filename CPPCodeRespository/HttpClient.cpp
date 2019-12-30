#include "HttpClient.h"
#include "LogWriter.h"

// ��ʼ��client��̬����
int HttpClient::s_exit_flag = 0;
ReqCallback HttpClient::s_req_callback;

// �ͻ��˵�����������Ӧ
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
			LogInfo("Error connecting to server, error code:{}", connect_status);
			s_exit_flag = 1;
		}
		break;
	case MG_EV_HTTP_REPLY:
	{
		LogInfo("Got reply:{}{}", (int)hm->body.len, hm->body.p);
		std::string rsp = std::string(hm->body.p, hm->body.len);
		connection->flags |= MG_F_SEND_AND_CLOSE;
		s_exit_flag = 1; // ÿ���յ������رձ������ӣ����ñ��

						 // �ص�����
		s_req_callback(rsp);
	}
	break;
	case MG_EV_CLOSE:
		if (s_exit_flag == 0)
		{
			LogInfo("Server closed connection");
			s_exit_flag = 1;
		};
		break;
	default:
		break;
	}
}


// ����һ�����󣬲��ص�����Ȼ��رձ�������,����дpost_data������ʱ��Ĭ��ʹ��get������д��ʹ��post����
void HttpClient::SendReq(const std::string &url, ReqCallback req_callback, const std::string &headers, const std::string &post_data)
{
	s_exit_flag = 0;
	// ���ص�������ֵ
	s_req_callback = req_callback;
	mg_mgr mgr;
	mg_mgr_init(&mgr, NULL);
	auto connection = mg_connect_http(&mgr, OnHttpEvent, url.c_str(), headers.c_str(), post_data.c_str());
	mg_set_protocol_http_websocket(connection);

	LogInfo("Send http request {}", url.c_str());

	while (s_exit_flag == 0)
		mg_mgr_poll(&mgr, 1000);

	mg_mgr_free(&mgr);
}