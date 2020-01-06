#pragma once
#include <windows.h>
#include <Dbghelp.h>
#include "strsafe.h"
#include "shlwapi.h"
#pragma comment( lib, "Dbghelp.lib" )
#pragma comment(lib,"shlwapi.lib")
/* 异常处理, Dump文件名 模块名称-年月日-时分秒-进程ID-线程ID */
LONG WINAPI UnhandledExceptionProc(struct _EXCEPTION_POINTERS *pExceptionPointers)
{
	HMODULE hModule = NULL;
	HANDLE hDumpFile;
	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR szNameAppend[128];
	TCHAR szBuild[128] = { 0 };
	TCHAR szError[512] = { 0 };
	SYSTEMTIME stLocalTime;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;

	SetErrorMode(SEM_NOGPFAULTERRORBOX);

	// 日期、时间
	StringCchPrintf(szBuild, 128, _T("Build: %s %s"), __DATE__, __TIME__);

	// 获取异常发生地址所在的模块
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)pExceptionPointers->ExceptionRecord->ExceptionAddress,
		&hModule);

	GetModuleFileName(hModule, szPath, MAX_PATH);

	// 构造错误信息
	StringCchPrintf(szError, 512,
		_T("Module=%s ,Code=%d ,Flag=%d ,Address=%p"),
		szPath,
		pExceptionPointers->ExceptionRecord->ExceptionCode,
		pExceptionPointers->ExceptionRecord->ExceptionFlags,
		pExceptionPointers->ExceptionRecord->ExceptionAddress);

	// 删除之前的DUMP文件，仅保留最近的3个
	if (TRUE)
	{
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		int count;
		GetModuleFileName(NULL, szPath, MAX_PATH);
		StringCchCat(szPath, MAX_PATH, L"*.dmp");

		// 计数
		hFind = FindFirstFile(szPath, &FindFileData);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			count = 0;
			do
			{
				count++;

			} while (FindNextFile(hFind, &FindFileData));

			FindClose(hFind);

			count -= 3;
			for (int i = 0; i < count; i++)
			{
				GetModuleFileName(NULL, szPath, MAX_PATH);
				StringCchCat(szPath, MAX_PATH, L"*.dmp");
				hFind = FindFirstFile(szPath, &FindFileData);
				if (hFind != INVALID_HANDLE_VALUE)
				{
					PathRemoveFileSpec(szPath);
					StringCchCat(szPath, MAX_PATH, L"\\");
					StringCchCat(szPath, MAX_PATH, FindFileData.cFileName);
					DeleteFile(szPath);
					FindClose(hFind);
				}
			}

		}
	}

	//构造文件名
	GetLocalTime(&stLocalTime);

	StringCchPrintf(szNameAppend, 128,
		_T("%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp"),
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
		GetCurrentProcessId(), GetCurrentThreadId());


	GetModuleFileName(NULL, szPath, MAX_PATH);
	StringCchCat(szPath, MAX_PATH, szNameAppend);

	//创建文件
	hDumpFile = CreateFile(szPath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	if (hDumpFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_USER_STREAM UserStream[2];
		MINIDUMP_USER_STREAM_INFORMATION UserInfo;
		UserInfo.UserStreamCount = 2;
		UserInfo.UserStreamArray = UserStream;
		UserStream[0].Type = CommentStreamW;
		UserStream[0].BufferSize = 128 * sizeof(TCHAR);
		UserStream[0].Buffer = szBuild;
		UserStream[1].Type = CommentStreamW;
		UserStream[1].BufferSize = 512 * sizeof(TCHAR);
		UserStream[1].Buffer = szError;

		ExpParam.ThreadId = GetCurrentThreadId();
		ExpParam.ExceptionPointers = pExceptionPointers;
		ExpParam.ClientPointers = TRUE;

		MINIDUMP_TYPE MiniDumpWithDataSegs = (MINIDUMP_TYPE)MiniDumpWithFullMemory;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
			hDumpFile, MiniDumpWithDataSegs, &ExpParam, &UserInfo, NULL);

		CloseHandle(hDumpFile);
	}
	return EXCEPTION_CONTINUE_SEARCH;
}