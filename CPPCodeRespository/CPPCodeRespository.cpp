// MongooseServer.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "HttpServer.h"
#include "JsonSerializable.h"
#include "HttpServerMultiThread.h"
#include "HttpClient.h"
#include <iostream>
#include "CPPEleven.h"
using namespace std;
//void print(string out)
//{
//	cout << out << endl;
//}
int main()
{
	//HttpServer* httpsServer = new HttpServer();
	//////HttpsTest 
	/*httpsServer->init("8888");
	httpsServer->startHttpService();*/

	////HttpTest
	//httpsServer->init("8442");
	//httpsServer->startHttpService();
	//delete httpsServer;

	////HttpClient
	//HttpClient *client = new HttpClient();
	//client->SendReq("127.0.0.1:8888", print);
	//delete client;

	//多线程测试
	//MultiThreadStart();

	int num = sumnum(333, 222);
	system("pause");
	return 0;
}

