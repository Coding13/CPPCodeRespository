// MongooseServer.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "HttpServer.h"
#include "JsonSerializable.h"
#include "HttpServerMultiThread.h"
#include "HttpClient.h"
#include <iostream>
#include "CPPEleven.h"
#include "LogWriter.h"
using namespace std;
void print(string out)
{
	cout << out << endl;
}
string getTime()
{
	char str[64];
	try
	{
		time_t timep;
		time(&timep);
		strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", localtime(&timep));
	}
	catch (std::exception e)
	{
	}
	return str;
}
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

	//HttpClient
	HttpClient *client = new HttpClient();
	cout << getTime() << endl;

	char temp[20];
	sprintf(temp, "%019d", i);
	string post_data = "<?xml version=\"1.0\" encoding=\"GBK\"?><Fpxx><Fpsj><Fp><Djh>" + string(temp) + "</Djh><Fpdm>201912251057</Fpdm><Fphm>12251057</Fphm><Gfmc>孙洪英</Gfmc><Gfsh>111222333456789</Gfsh><Gfyhzh>6217000010005805406&amp;</Gfyhzh><Gfdzdh>&amp;XSFDZDH</Gfdzdh><Bz/><Fhr/><Skr/><Spbmbbh>34.2</Spbmbbh><Hsbz>0</Hsbz><Spxx><Sph><Xh>1</Xh><Spmc>的撒发生发生11</Spmc><Ggxh/><Jldw/><Spbm>1010101050000000000</Spbm><Qyspbm>30</Qyspbm><Syyhzcbz>0</Syyhzcbz><Lslbz/><Yhzcsm/><Kce/><Dj/><Sl/><Je>0.94</Je><Se>0.06</Se><Slv>0.06</Slv></Sph></Spxx><Kpr>金极为</Kpr><Xfmc>422010201709012004</Xfmc><Xfsh>422010201709012004</Xfsh><Xfdzdh>北京市海淀区北清路68号 62436800</Xfdzdh><Xfyhzh>海淀区西北旺建设银行支行 6243778866554433</Xfyhzh></Fp></Fpsj></Fpxx>";
	client->SendReq("https://127.0.0.1:8888", print, "", post_data);

	delete client;
	cout << getTime() << endl;

	//initLoggerWriter();
	//loggerWriter->info("test logger writer");
	//多线程测试
	//MultiThreadStart();
	/*int num = sumnum(333, 222);
	system("pause");*/
	return 0;
}

