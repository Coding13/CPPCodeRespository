#include "HttpServer.h"
#include "spdlog/spdlog.h"
#include "spdlog/async.h" //support for async logging
#include "spdlog/sinks/daily_file_sink.h"
#include "LogWriter.h"
#ifdef _DEBUG
#pragma comment(lib,"Lib\\libeay32MDd.lib")
#pragma comment(lib,"Lib\\ssleay32MDd.lib")
#else
#pragma comment(lib,"Lib\\libeay32MD.lib")
#pragma comment(lib,"Lib\\ssleay32MD.lib")
#endif
namespace spd = spdlog;
//静态变量需要在使用前定义 否则lnk2001
std::unordered_map<std::string, ReqHandler> HttpServer::s_handler_map;
mg_mgr HttpServer::mgr;
void HttpServer::init(const std::string port)
{
	m_port = port;
}
void HttpServer::startHttpService(std::string certName,std::string keyName)
{
	struct mg_connection *nc;
	struct mg_bind_opts bind_opts;
	const char *err;

	mg_mgr_init(&mgr, NULL);
	memset(&bind_opts, 0, sizeof(bind_opts));
	bind_opts.ssl_cert = certName.c_str();
	bind_opts.ssl_key = keyName.c_str();
	bind_opts.error_string = &err;

	LogInfo("Starting Https server on port:{}, cert:{}, key:{}", m_port.c_str(), bind_opts.ssl_cert, bind_opts.ssl_key);
	nc = mg_bind_opt(&mgr, m_port.c_str(), ev_handler, bind_opts);
	if (nc == NULL) 
	{
		LogInfo("Failed to create listener:{}", err);
		return ;
	}

	mg_set_protocol_http_websocket(nc);

	//通过调用循环创建一个事件mg_mgr_poll()循环
	for (;;)
	{
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);
}
void HttpServer::startHttpService()
{
	struct mg_mgr mgr;
	struct mg_connection *nc;
	struct mg_bind_opts bind_opts;
	const char *err;

	mg_mgr_init(&mgr, NULL);
	memset(&bind_opts, 0, sizeof(bind_opts));
	bind_opts.error_string = &err;

	LogInfo("Starting Http server on port:{}", m_port.c_str());
	nc = mg_bind_opt(&mgr, m_port.c_str(), ev_handler, bind_opts);
	if (nc == NULL) 
	{
		LogInfo("Failed to create listener:{}", err);
		return ;
	}

	mg_set_protocol_http_websocket(nc);

	//通过调用循环创建一个事件mg_mgr_poll()循环
	for (;;)
	{
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);
}

bool HttpServer::route_check(http_message *http_msg,const char *route_prefix)
{
	if (mg_vcmp(&http_msg->uri, route_prefix) == 0)
		return true;
	else
		return false;
}
//web server
void HttpServer::HandleHttpEvent(mg_connection *connection, http_message *http_req)
{
	//std::string req_str = std::string(http_req->message.p, http_req->message.len);
	//LogInfo("got request: %s\n", req_str.c_str());

	//// 先过滤是否已注册的函数回调
	//std::string url = std::string(http_req->uri.p, http_req->uri.len);
	//std::string body = std::string(http_req->body.p, http_req->body.len);
	//auto it = s_handler_map.find(url);
	//if (it != s_handler_map.end())
	//{
	//	ReqHandler handle_func = it->second;
	//	handle_func(url, body, connection, SendData);
	//}

	//// 其他请求
	//if (route_check(http_req, "/"))
	//{
	//	static mg_serve_http_opts s_server_option;
	//	mg_serve_http(connection, http_req, s_server_option);
	//}
	//else if (route_check(http_req, "/api/hello"))
	//{
	//	// 直接回传
	//	SendData(connection, "welcome to httpserver");
	//}
	//else if (route_check(http_req, "/api/sum"))
	//{
	//	// 简单post请求，加法运算测试
	//	char n1[100], n2[100];
	//	double result;

	//	/* Get form variables */
	//	mg_get_http_var(&http_req->body, "n1", n1, sizeof(n1));
	//	mg_get_http_var(&http_req->body, "n2", n2, sizeof(n2));

	//	/* Compute the result and send it back as a JSON object */
	//	result = strtod(n1, NULL) + strtod(n2, NULL);
	//	SendData(connection, std::to_string(result));
	//}
	//else
	//{
	//	mg_printf(
	//		connection,
	//		"%s",
	//		"HTTP/1.1 501 Not Implemented\r\n"
	//		"Content-Length: 0\r\n\r\n");
	//}
}
void HttpServer::ev_handler(struct mg_connection *nc, int ev, void *p) 
{
	LogInfo("ev_handler event:{}", ev);
	switch (ev)
	{
	case MG_EV_ACCEPT:
		break;
	case MG_EV_CLOSE:
		break;
	case MG_EV_CONNECT:
		break;
	case MG_EV_HTTP_CHUNK:
		break;
	case MG_EV_HTTP_REPLY:
		break;
	case MG_EV_HTTP_REQUEST:
		{
			//mg_serve_http(nc, (struct http_message *) p, s_http_server_opts);
			struct http_message *hm = (struct http_message *)p;

			//请求方式 get/post
			std::string strMethod(hm->method.p, (hm->method.p + hm->method.len));
			LogInfo("Method:{}", strMethod.c_str());
			//请求体
			std::string strbody(hm->body.p, (hm->body.p + hm->body.len));
			LogInfo("Body:{}", strbody.c_str());
			//请求uri
			std::string struri(hm->uri.p, (hm->uri.p + hm->uri.len));
			LogInfo("Uri:{}", struri.c_str());
			if (struri == "close" || struri=="/close")
			{
				HttpServer::Close(mgr);
				break;
			}
			int i=0;
			while(hm->header_names[i].p != NULL && hm->header_values[i].p != NULL)
			{
				std::string strKey(hm->header_names[i].p, (hm->header_names[i].p + hm->header_names[i].len));
				std::string strValue(hm->header_values[i].p, (hm->header_values[i].p + hm->header_values[i].len));
				LogInfo("Header:{} {}" , strKey.c_str(), strValue.c_str());
				i++;
			}
			//std::string struri(hm->header_names, (hm->uri.p + hm->uri.len));

			SendData(nc, "Hello world");
		}
		break;
	default:
		break;
	}
}
void HttpServer::AddHandler(const std::string &url, ReqHandler req_handler)
{
	if (s_handler_map.find(url) != s_handler_map.end())
		return;

	s_handler_map.insert(std::make_pair(url, req_handler));
}
void HttpServer::RemoveHandler(const std::string &url)
{
	auto it = s_handler_map.find(url);
	if (it != s_handler_map.end())
		s_handler_map.erase(it);
}
//支持跨域
void HttpServer::SendData(mg_connection * nc, const char* sdata)
{
	if (!nc || strlen(sdata)<= 0)
	{
		mg_http_send_error(nc, 400, "Error : bad request");
		return;
	}
	
	mg_send_head(nc, 200,strlen(sdata), "Access-Control-Allow-Origin: *");
	mg_send(nc, sdata, strlen(sdata));
	LogInfo("SendData: {}", sdata);
}
//支持跨域
void HttpServer::SendResponse(mg_connection *connection,const char* rsp)
{
	//必须先发送header, 也可以用HTTP/2.0
	mg_printf(connection, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
	//以json形式返回
	mg_printf_http_chunk(connection, "{ \"result\":\"%s\" }", rsp);
	//发送空白字符块，结束当前响应
	mg_send_http_chunk(connection, "", 0);
	connection->flags |= MG_F_SEND_AND_CLOSE;
	LogInfo("SendResponse {}", rsp);
}
bool HttpServer::Close(mg_mgr mgr)
{
	mg_mgr_free(&mgr);
	return true;
}