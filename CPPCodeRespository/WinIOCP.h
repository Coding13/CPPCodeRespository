#pragma once
#ifndef _WINIOCP_INCLUDE_
#define _WININCP_INCOUDE_

#include"winerror.h"
#include"Winsock2.h"
#pragma comment(lib, "ws2_32")
#include"windows.h"
#include<iostream>
using namespace std;

/// �궨��
#define PORT 5050
#define DATA_BUFSIZE 8192

#define OutErr(a) cout << (a) << endl \
      << "������룺"<< WSAGetLastError() << endl \
      << "�����ļ���"<< __FILE__ << endl  \
      << "����������"<< __LINE__ << endl \

#define OutMsg(a) cout << (a) << endl;


/// ȫ�ֺ�������


///////////////////////////////////////////////////////////////////////
//
// ������       : InitWinsock
// ��������     : ��ʼ��WINSOCK
// ����ֵ       : void
//
///////////////////////////////////////////////////////////////////////
void InitWinsock()
{
	// ��ʼ��WINSOCK
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		OutErr("WSAStartup()");
	}
}

///////////////////////////////////////////////////////////////////////
//
// ������       : BindServerOverlapped
// ��������     : �󶨶˿ڣ�������һ�� Overlapped ��ListenSocket
// ����         : int nPort
// ����ֵ       : SOCKET
//
///////////////////////////////////////////////////////////////////////
SOCKET BindServerOverlapped(int nPort)
{
	// ����socket  ע�������и�WSA_FLAG_OVERLAPPED����    ֻ���ڷ���������Ҫ��ô�����ڿͻ����ǲ���Ҫ��
	SOCKET sServer = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	// �󶨶˿�
	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(nPort);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (::bind(sServer, (struct sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		OutErr("bind Failed!");
		return NULL;
	}

	// ���ü�������Ϊ200
	if (listen(sServer, 200) != 0)
	{
		OutErr("listen Failed!");
		return NULL;
	}
	return sServer;
}


/// �ṹ�嶨��
typedef struct
{
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	CHAR Buffer[DATA_BUFSIZE];
}PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA;


typedef struct
{
	SOCKET Socket;
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;


DWORD WINAPI ProcessIO(LPVOID lpParam)
{
	HANDLE CompletionPort = (HANDLE)lpParam;
	DWORD BytesTransferred;
	LPPER_HANDLE_DATA PerHandleData;
	LPPER_IO_OPERATION_DATA PerIoData;

	while (true)
	{

		if (0 == GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (LPDWORD)&PerHandleData, (LPOVERLAPPED*)&PerIoData, INFINITE))
		{
			if ((GetLastError() == WAIT_TIMEOUT) || (GetLastError() == ERROR_NETNAME_DELETED))
			{
				cout << "closingsocket" << PerHandleData->Socket << endl;
				closesocket(PerHandleData->Socket);

				delete PerIoData;
				delete PerHandleData;
				continue;
			}
			else
			{
				OutErr("GetQueuedCompletionStatus failed!");
			}
			return 0;
		}

		// ˵���ͻ����Ѿ��˳�
		if (BytesTransferred == 0)
		{
			cout << "closing socket" << PerHandleData->Socket << endl;
			closesocket(PerHandleData->Socket);
			delete PerIoData;
			delete PerHandleData;
			continue;
		}

		// ȡ�����ݲ�����
		cout << PerHandleData->Socket << "���͹�������Ϣ��" << PerIoData->Buffer << endl;

		// ������ socket Ͷ��WSARecv����
		DWORD Flags = 0;
		DWORD dwRecv = 0;
		ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
		PerIoData->DataBuf.buf = PerIoData->Buffer;
		PerIoData->DataBuf.len = DATA_BUFSIZE;
		WSARecv(PerHandleData->Socket, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
	}

	return 0;
}
#include<stdio.h>
#include<Winsock2.h>
#define MAXCNT 30000
void WinIOCPClient()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);//WSAStartup()�����׽��ֿ�
	if (err != 0) {

		return;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return;
	}

	static int nCnt = 0;
	char sendBuf[2000];
	//     char recvBuf[100];
	while (nCnt < MAXCNT)
	{
		SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
		SOCKADDR_IN addrSrv;
		addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//���ػ�·��ַ127������һ̨�����ϲ��Ե�IP
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_port = htons(5050);//�ͷ������˵Ķ˿ںű���һ��
		connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));//���ӷ������ˣ��׽��֣���ַת�������ȣ�


		sprintf(sendBuf, "This is TestNo : %d\n", ++nCnt);
		send(sockClient, sendBuf, strlen(sendBuf) + 1, 0);//��������˷������ݣ�"+1"��Ϊ�˸�'\0'���ռ�
		printf("send:%s", sendBuf);

		//           memset(recvBuf,0,100);
		//           recv(sockClient,recvBuf,100,0);//��������
		//           printf("%s\n",recvBuf);//��ӡ

		closesocket(sockClient);//�ر��׽��֣��ͷ�Ϊ����׽��ַ������Դ
		Sleep(1);
	}
	WSACleanup();//��ֹ������׽��ֿ��ʹ��
}
void WinIOCPServer()
{
	InitWinsock();
	HANDLE CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	//����ϵͳ��CPU�������������߳�
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	//�߳���Ŀ=ϵͳ������Ŀ������.
	for (int i = 0; i <SystemInfo.dwNumberOfProcessors * 2; i++)
	{
		HANDLE hProcessIO = CreateThread(NULL, 0, ProcessIO, CompletionPort, 0, NULL);
		if (hProcessIO)
		{
			CloseHandle(hProcessIO);
		}
	}

	//��������SOCKET
	SOCKET sListen = BindServerOverlapped(PORT);

	SOCKET sClient;
	LPPER_HANDLE_DATA PerHandleData;
	LPPER_IO_OPERATION_DATA PerIoData;
	while (true)
	{
		// �ȴ��ͻ��˽���
		//sClient = WSAAccept(sListen, NULL, NULL, NULL, 0);
		sClient = accept(sListen, 0, 0);
		cout << "Socket " << sClient << "���ӽ���" << endl;

		PerHandleData = new PER_HANDLE_DATA();
		PerHandleData->Socket = sClient;

		// ������Ŀͻ��˺���ɶ˿���ϵ����
		CreateIoCompletionPort((HANDLE)sClient, CompletionPort, (DWORD)PerHandleData, 0);

		// ����һ��Overlapped����ʹ�����Overlapped�ṹ��socketͶ�ݲ���
		PerIoData = new PER_IO_OPERATION_DATA();

		ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
		PerIoData->DataBuf.buf = PerIoData->Buffer;
		PerIoData->DataBuf.len = DATA_BUFSIZE;

		// Ͷ��һ��WSARecv����
		DWORD Flags = 0;
		DWORD dwRecv = 0;
		WSARecv(sClient, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
	}

	DWORD dwByteTrans;
	//��һ���Ѿ���ɵ�IO֪ͨ��ӵ�IO��ɶ˿ڵĶ�����.
	//�ṩ�����̳߳��е������߳�ͨ�ŵķ�ʽ.
	PostQueuedCompletionStatus(CompletionPort, dwByteTrans, 0, 0);  //IO�������ʱ���յ��ֽ���.

	closesocket(sListen);
}
#endif