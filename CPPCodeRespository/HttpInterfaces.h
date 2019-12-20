#pragma once
#ifndef __HTTPSERVER_INCLUDE__
#define __HTTPSERVER_INCLUDE__

//dll导出相关定义
#define DLL_EXPORTS
#ifdef  DLL_EXPORTS
#define DLL_API extern "C" __declspec(dllexport)//注意decl前面是两个下划线
#else
#define DLL_API __declspec(dllimport)
#endif

#define MG_ENABLE_SSL 1
#include "mongoose.h"

DLL_API void __stdcall Init();
//初始化Http服务
//绑定事件处理函数 绑定端口 并建立连接
DLL_API int __stdcall  Bind(mg_event_handler_t callback, const char * port, const char* certName, const char* keyName, char ** errormsg);
//关闭连接
DLL_API void __stdcall Close();

//回传消息
DLL_API void __stdcall SendResponse(mg_connection *connection, const char * respMsg);
//回传失败的消息
DLL_API void __stdcall SendErrorResponse(mg_connection *connection, int errCode, const char * errMsg);
DLL_API void __stdcall StartListen(int timeOut);
#endif