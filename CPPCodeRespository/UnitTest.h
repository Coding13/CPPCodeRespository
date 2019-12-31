#pragma once
#include "HttpClient.h"
#include "Base64Convert.h"
#include <iostream>
#include"HttpServer.h"
#include"CPPEleven.h"
#include"LogWriter.h"
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

void HttpCientTest()
{
	//HttpClient
	HttpClient *client = new HttpClient();
	cout << getTime() << endl;
	for (int i = 0; i < 1000; i++)
	{
		char temp[20];
		sprintf(temp, "1%018d", i);
		string post_data = "<?xml version=\"1.0\" encoding=\"GBK\"?><Fpxx><Fpsj><Fp><Djh>" + string(temp) + "</Djh><Fpdm>201912251057</Fpdm><Fphm>12251057</Fphm><Gfmc>孙洪英</Gfmc><Gfsh>111222333456789</Gfsh><Gfyhzh>6217000010005805406</Gfyhzh><Gfdzdh>XSFDZDH</Gfdzdh><Bz/><Fhr/><Skr/><Spbmbbh>34.2</Spbmbbh><Hsbz>0</Hsbz><Spxx><Sph><Xh>1</Xh><Spmc>测试使用的商品名称</Spmc><Ggxh/><Jldw/><Spbm>1010101050000000000</Spbm><Qyspbm>30</Qyspbm><Syyhzcbz>0</Syyhzcbz><Lslbz/><Yhzcsm/><Kce/><Dj/><Sl/><Je>0.94</Je><Se>0.06</Se><Slv>0.06</Slv></Sph></Spxx><Kpr>金极为</Kpr><Xfmc>422010201709012004</Xfmc><Xfsh>422010201709012004</Xfsh><Xfdzdh>北京市海淀区 62436800</Xfdzdh><Xfyhzh>海淀区西北旺建设银行支行 6243778866554433</Xfyhzh></Fp></Fpsj></Fpxx>";
		string enc;
		Base64Encode(post_data, enc);
		client->SendReq("https://127.0.0.1:8888", print, "", enc);
	}
	delete client;
	cout << getTime() << endl;
}
void HttpServerTest()
{
	HttpServer* httpsServer = new HttpServer();
	//HttpsTest 
	httpsServer->init("8888");
	httpsServer->startHttpService("server.pem","server.key");
	delete httpsServer;

	//多线程测试
	//MultiThreadStart();
}
string base64TestInput = "<?xml version=\"1.0\" encoding=\"GBK\"?><Fpxx><Fpsj><Fp><Djh>20191230</Djh><Fpdm>201912251057</Fpdm><Fphm>12251057</Fphm><Gfmc>孙洪英</Gfmc><Gfsh>111222333456789</Gfsh><Gfyhzh>6217000010005805406</Gfyhzh><Gfdzdh>XSFDZDH</Gfdzdh><Bz/><Fhr/><Skr/><Spbmbbh>34.2</Spbmbbh><Hsbz>0</Hsbz><Spxx><Sph><Xh>1</Xh><Spmc>测试使用的商品名称</Spmc><Ggxh/><Jldw/><Spbm>1010101050000000000</Spbm><Qyspbm>30</Qyspbm><Syyhzcbz>0</Syyhzcbz><Lslbz/><Yhzcsm/><Kce/><Dj/><Sl/><Je>0.94</Je><Se>0.06</Se><Slv>0.06</Slv></Sph></Spxx><Kpr>金极为</Kpr><Xfmc>422010201709012004</Xfmc><Xfsh>422010201709012004</Xfsh><Xfdzdh>北京市海淀区 62436800</Xfdzdh><Xfyhzh>海淀区西北旺建设银行支行 6243778866554433</Xfyhzh></Fp></Fpsj></Fpxx>";
void base64Test()
{
	string enc;
	Base64Encode(base64TestInput, enc);
	cout << enc;
}
void CppElevenTest()
{
	int num = sumnum(333, 222);
}
void LogWriterTest()
{
	for (int i=0;i<900000;i++)
	{
		LogInfo("Log writter test:{}" , i);
	}
}
struct LinkNode
{
	LinkNode *Next;
	int value;
};
LinkNode * LinkReverse(LinkNode *pHead)
{
	LinkNode * pRevHead;
	LinkNode * pNode = pHead;
	LinkNode * pPre = nullptr;
	while (pNode!=nullptr)
	{
		LinkNode * pNext = pNode->Next;
		if (pNode->Next != nullptr)
			pRevHead = pNode->Next;
		pNode->Next = pPre;
		pPre = pNode;
		pNode = pNext;
	}
	return pRevHead;
}
void TestReverse()
{
	LinkNode * link = new LinkNode;
	LinkNode * head = link;
	for (int i = 1; i < 10; i++)
	{
		link->value = i;
		link->Next = new LinkNode;
		link = link->Next;
	}
	link->Next = nullptr;
	LinkNode* rev = LinkReverse(head);
}