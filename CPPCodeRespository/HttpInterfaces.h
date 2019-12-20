#pragma once
#ifndef __HTTPSERVER_INCLUDE__
#define __HTTPSERVER_INCLUDE__

//dll������ض���
#define DLL_EXPORTS
#ifdef  DLL_EXPORTS
#define DLL_API extern "C" __declspec(dllexport)//ע��declǰ���������»���
#else
#define DLL_API __declspec(dllimport)
#endif

#define MG_ENABLE_SSL 1
#include "mongoose.h"

DLL_API void __stdcall Init();
//��ʼ��Http����
//���¼������� �󶨶˿� ����������
DLL_API int __stdcall  Bind(mg_event_handler_t callback, const char * port, const char* certName, const char* keyName, char ** errormsg);
//�ر�����
DLL_API void __stdcall Close();

//�ش���Ϣ
DLL_API void __stdcall SendResponse(mg_connection *connection, const char * respMsg);
//�ش�ʧ�ܵ���Ϣ
DLL_API void __stdcall SendErrorResponse(mg_connection *connection, int errCode, const char * errMsg);
DLL_API void __stdcall StartListen(int timeOut);
#endif