#pragma once
#include<windows.h> 
#include <functional>
#include <WinInet.h>
#include"tchar.h"
#pragma comment(lib, "Wininet.lib")

enum DOWNLOADSTATE { DOWNLOAD_SUCCESS, DOWNLOAD_EMPTY, DOWNLOAD_ERROR, DOWNLOAD_TIMEOUT };
typedef std::function< VOID(ULONGLONG ullTotalSize, ULONGLONG ullCurrentSize, float fPercent) > FHttpDownNotify;
class CHttpDownLoad
{
public:
	CHttpDownLoad() {};
	DOWNLOADSTATE DownLoad(LPCTSTR szURL, LPCTSTR szFilePath, DWORD dwTimeOut, FHttpDownNotify fNotfiy)
	{
		for (int i = 0; i<3; i++)
		{
			OutputDebugString(L"DownLoad .......................");
			if (DOWNLOAD_SUCCESS == _Download(szURL, szFilePath, dwTimeOut, fNotfiy))
			{
				return DOWNLOAD_SUCCESS;
			}
		}
		OutputDebugString(L"DownLoad .......................Error............");
		return DOWNLOAD_ERROR;
		//return _Download( szURL, szFilePath, dwTimeOut, fNotfiy);
	}

private:
	struct THREAD_PARAM
	{
		TCHAR szURL[1024];
		TCHAR szPath[1024];
		HINTERNET hSession;
		HINTERNET hRequest;
		HINTERNET hConnect;
		BOOL bRet;
	};
private:
	DOWNLOADSTATE _Download(LPCTSTR szURL, LPCTSTR szFilePath, DWORD dwTimeOut, FHttpDownNotify fNotfiy)
	{
		if (NULL == szURL)
		{
			return DOWNLOAD_ERROR;
		}

		// 必须清除IE缓存
		DeleteUrlCacheEntry(szURL);

		HINTERNET hSession = InternetOpen(L"",
			INTERNET_OPEN_TYPE_PRECONFIG,
			NULL,
			NULL,
			0);
		if (NULL == hSession)
		{
			OutputDebugString(L"StartDownload 创建Session失败\n");
			return DOWNLOAD_ERROR;
		}

		THREAD_PARAM threadParm;
		ZeroMemory(&threadParm, sizeof(THREAD_PARAM));
		_tcscpy_s(threadParm.szURL, 1024 - 1, szURL);
		if (szFilePath != NULL)
		{
			_tcscpy_s(threadParm.szPath, 1024 - 1, szFilePath);
		}
		threadParm.hSession = hSession;

		HANDLE hThread = CreateThread(
			NULL,            // Pointer to thread security attributes 
			0,               // Initial thread stack size, in bytes 
			WorkerFunction,  // Pointer to thread function 
			&threadParm,     // The argument for the new thread
			0,               // Creation flags 
			NULL     // Pointer to returned thread identifier 
		);

		// Wait for the call to InternetConnect in worker function to complete
		if (WaitForSingleObject(hThread, dwTimeOut) == WAIT_TIMEOUT)
		{
			OutputDebugString(L"StartDownload 与服务器通讯超时\n");
			InternetCloseHandle(hSession);
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
			if (NULL != threadParm.hConnect)
			{
				InternetCloseHandle(threadParm.hConnect);
			}
			if (NULL != threadParm.hRequest)
			{
				InternetCloseHandle(threadParm.hRequest);
			}
			return DOWNLOAD_TIMEOUT;
		}
		CloseHandle(hThread);

		if (!threadParm.bRet)
		{
			OutputDebugString(L"StartDownload 与服务器通讯失败\n");
			InternetCloseHandle(hSession);
			if (NULL != threadParm.hConnect)
			{
				InternetCloseHandle(threadParm.hConnect);
			}
			if (NULL != threadParm.hRequest)
			{
				InternetCloseHandle(threadParm.hRequest);
			}
			return DOWNLOAD_ERROR;
		}

		DWORD dwStatusCode = 0;
		DWORD dwBufLen = sizeof(dwStatusCode);
		if (!HttpQueryInfo(threadParm.hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwStatusCode, &dwBufLen, NULL))
		{
			OutputDebugString(L"StartDownload 查询状态代码失败\n");
			InternetCloseHandle(hSession);
			InternetCloseHandle(threadParm.hConnect);
			InternetCloseHandle(threadParm.hRequest);
			return DOWNLOAD_ERROR;
		}
		if (dwStatusCode != 200)
		{
			OutputDebugString(L"StartDownload 状态代码非OK\n");
			InternetCloseHandle(hSession);
			InternetCloseHandle(threadParm.hConnect);
			InternetCloseHandle(threadParm.hRequest);
			return DOWNLOAD_ERROR;
		}

		if (!szFilePath)
		{
			InternetCloseHandle(hSession);
			InternetCloseHandle(threadParm.hConnect);
			InternetCloseHandle(threadParm.hRequest);
			return DOWNLOAD_SUCCESS;
		}

		// 接收服务器返回
		DWORD dwBytesToRead = 0;
		dwBufLen = sizeof(dwBytesToRead);
		if (!HttpQueryInfo(threadParm.hRequest,
			HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
			(LPVOID)&dwBytesToRead,
			&dwBufLen,
			NULL))
		{
			OutputDebugString(L"StartDownload 查询文件大小失败\n");
			InternetCloseHandle(hSession);
			InternetCloseHandle(threadParm.hConnect);
			InternetCloseHandle(threadParm.hRequest);
			return DOWNLOAD_ERROR;
		}

		if (0 == dwBytesToRead)
		{
			OutputDebugString(L"StartDownload 查询文件大小失败\n");
			InternetCloseHandle(hSession);
			InternetCloseHandle(threadParm.hConnect);
			InternetCloseHandle(threadParm.hRequest);
			return DOWNLOAD_EMPTY;
		}

		HANDLE hFile = CreateFile(szFilePath,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			OutputDebugString(L"StartDownload 创建文件失败\n");
			InternetCloseHandle(hSession);
			InternetCloseHandle(threadParm.hConnect);
			InternetCloseHandle(threadParm.hRequest);
			return DOWNLOAD_ERROR;
		}

		DWORD dwRead = 0;
		DWORD dwWrite = 0;
		DWORD dwTotal = 0;

		if (fNotfiy)
		{
			char szRecvBuf[1024] = { 0 };
			while (InternetReadFile(threadParm.hRequest, szRecvBuf, 1024, &dwRead))
			{
				if (0 == dwRead)
				{
					OutputDebugString(L"InternetReadFile 0 ==dwRead break");
					break;
				}

				BOOL bRet = WriteFile(hFile, szRecvBuf, dwRead, &dwWrite, NULL);
				if (!bRet || dwRead != dwWrite)
				{
					OutputDebugString(L"InternetReadFile dwRead != dwWrite");
					break;
				}
				dwTotal = dwTotal + dwWrite;

				// 返回下载进度。。。
				if (NULL != fNotfiy)
				{
					float f = (float)dwTotal / dwBytesToRead;
					fNotfiy(dwBytesToRead, dwTotal, f);
				}
			}

			if (dwTotal != dwBytesToRead)
			{
				OutputDebugString(L"StartDownload 接收数据失败\n");
				CloseHandle(hFile);
				InternetCloseHandle(hSession);
				InternetCloseHandle(threadParm.hConnect);
				InternetCloseHandle(threadParm.hRequest);
				DeleteFile(szFilePath);
				return DOWNLOAD_ERROR;
			}
		}
		else
		{
			char *szRecvBuf = new char[dwBytesToRead];
			ZeroMemory(szRecvBuf, dwBytesToRead);
			BOOL bRet = InternetReadFile(threadParm.hRequest, szRecvBuf, dwBytesToRead, &dwRead);
			if (bRet)
			{
				bRet = WriteFile(hFile, szRecvBuf, dwRead, &dwWrite, NULL);
				if (!bRet)
				{
					OutputDebugString(L"StartDownload 写入数据失败\n");
					CloseHandle(hFile);
					InternetCloseHandle(hSession);
					InternetCloseHandle(threadParm.hConnect);
					InternetCloseHandle(threadParm.hRequest);
					DeleteFile(szFilePath);
					delete[] szRecvBuf;
					return DOWNLOAD_ERROR;
				}
			}
			delete[] szRecvBuf;
		}
		CloseHandle(hFile);
		InternetCloseHandle(hSession);
		InternetCloseHandle(threadParm.hConnect);
		InternetCloseHandle(threadParm.hRequest);
		return DOWNLOAD_SUCCESS;
	}


private:
	static DWORD WINAPI WorkerFunction(LPVOID lpParam)
	{
		THREAD_PARAM* worker = (THREAD_PARAM*)lpParam;

		URL_COMPONENTS uc;
		TCHAR szScheme[1024] = { 0 };
		TCHAR szHostName[1024] = { 0 };
		TCHAR szUserName[1024] = { 0 };
		TCHAR szPassword[1024] = { 0 };
		TCHAR szUrlPath[1024] = { 0 };
		TCHAR szExtraInfo[1024] = { 0 };

		uc.dwStructSize = sizeof(uc);
		uc.lpszScheme = szScheme;
		uc.lpszHostName = szHostName;
		uc.lpszUserName = szUserName;
		uc.lpszPassword = szPassword;
		uc.lpszUrlPath = szUrlPath;
		uc.lpszExtraInfo = szExtraInfo;

		uc.dwSchemeLength = 1024;
		uc.dwHostNameLength = 1024;
		uc.dwUserNameLength = 1024;
		uc.dwPasswordLength = 1024;
		uc.dwUrlPathLength = 1024;
		uc.dwExtraInfoLength = 1024;
		BOOL bRet = InternetCrackUrl(worker->szURL, 0, 0, &uc);
		if (!bRet)
		{
			worker->bRet = FALSE;
			worker->hConnect = NULL;
			worker->hRequest = NULL;

			OutputDebugString(L"InternetCrackUrl失败\n");
			return 0;
		}

		HINTERNET hConnect = InternetConnect(worker->hSession,
			uc.lpszHostName,
			uc.nPort,
			NULL,
			NULL,
			INTERNET_SERVICE_HTTP,
			0,
			0);
		if (NULL == hConnect)
		{
			worker->bRet = FALSE;
			worker->hConnect = NULL;
			worker->hRequest = NULL;
			OutputDebugString(L"InternetConnect失败\n");
			return 0;
		}
		else
		{
			worker->hConnect = hConnect;
		}


		TCHAR szUrl[1024] = { 0 };
		_tcscat_s(szUrl, _countof(szUrl), uc.lpszUrlPath);
		_tcscat_s(szUrl, _countof(szUrl), uc.lpszExtraInfo);

		CONST TCHAR *szAcceptType[2] = { _T("Accept: */*"),NULL };
		HINTERNET hRequest = HttpOpenRequest(hConnect,
			L"GET",
			szUrl,
			L"HTTP/1.1",
			NULL,
			(const TCHAR **)&szAcceptType,
			0,
			0);
		if (NULL == hRequest)
		{
			worker->hRequest = NULL;
			worker->bRet = FALSE;
			OutputDebugString(L"HttpOpenRequest失败\n");
			return 0;
		}
		else
		{
			worker->hRequest = hRequest;
		}

		if (!HttpSendRequest(hRequest, NULL, 0, NULL, 0))
		{
			worker->bRet = FALSE;
			OutputDebugString(L"HttpSendRequest失败\n");
			return 0;
		}
		worker->bRet = TRUE;
		return 0;
	}

};