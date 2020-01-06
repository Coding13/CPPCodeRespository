#pragma once
//服务控制函数
#include <Windows.h>
#include "tlhelp32.h"
#include "stdio.h"
#include "tchar.h "
//全局变量
SERVICE_STATUS ssStatus;				 // 当前服务的状态
DWORD          dwErr = 0;				 // 错误码
HANDLE		   hServerStopEvent = NULL;	 // 服务停用事件
SERVICE_STATUS_HANDLE   sshStatusHandle; // 服务状态控制句柄
HANDLE	hRpcThdHandle = NULL;			 // Rpc服务虚拟化接收线程

#define SZ_SERVICE_NAME  L"ServiceTest"
#define	SZ_SERVICE_DISPLAY_NAME		L"ServiceTestForDisplay"

//服务入口
void ServiceInit(const char **argv)
{
	SERVICE_TABLE_ENTRY dispatchTable[] =
	{
		{ (LPWSTR)L"SZ_SERVICE_NAME", (LPSERVICE_MAIN_FUNCTION)ServiceMain },{ NULL, NULL }
	};
	if (_stricmp(argv[1], "-service") == 0)
	{
		CmdInstallService();
		return;
	}
	else if (_stricmp(argv[1], "-remove") == 0)
	{
		CmdRemoveService();
		return;
	}
	StartServiceCtrlDispatcher(dispatchTable);
}
//移除服务
void CmdRemoveService()
{
	SC_HANDLE   schService, schSCManager;
	schSCManager = OpenSCManager(
		NULL,                   // machine (NULL == local)
		NULL,                   // database (NULL == default)
		SC_MANAGER_ALL_ACCESS   // access required
	);
	if (schSCManager)
	{
		schService = OpenService(schSCManager, SZ_SERVICE_NAME, SERVICE_ALL_ACCESS);
		if (schService)
		{
			// try to stop the service
			if (ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus))
			{
				OutputDebugStringW(L"\nStopping FVM Controller Service.");
				Sleep(1000);

				while (QueryServiceStatus(schService, &ssStatus))
				{
					if (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
					{
						OutputDebugStringW(L".");
						Sleep(1000);
					}
					else
					{
						break;
					}
				}

				if (ssStatus.dwCurrentState == SERVICE_STOPPED)
				{
					OutputDebugStringW(L"\nController Service stopped.\n");
				}
				else
				{
					OutputDebugStringW(L"\nFailed to stop Controller Service.\n");
				}
			}

			// now remove the service
			if (DeleteService(schService))
			{
				OutputDebugStringW(L"Controller Service removed.\n");
			}
			else
			{
				OutputDebugStringW(L"Delete Controller Service failed.\n");
			}

			CloseServiceHandle(schService);
		}
		else
		{
			OutputDebugStringW(L"Open Controller Service failed.\n");
		}

		CloseServiceHandle(schSCManager);
	}
	else
	{
		OutputDebugStringW(L"Open SCManager failed.\n");
	}
}

//启动服务
VOID CmdStartService(SC_HANDLE hservice)
{
	if (!StartService(hservice, 0, NULL))
	{
		OutputDebugStringW(L"\nStarting FVM Controller Service failed.\n");
		return;
	}

	OutputDebugStringW(L"Starting Controller Service.");
	Sleep(1000);

	while (QueryServiceStatus(hservice, &ssStatus))
	{
		if (ssStatus.dwCurrentState == SERVICE_START_PENDING)
		{
			OutputDebugStringW(L".");
			Sleep(1000);
		}
		else
		{
			break;
		}
	}

	if (ssStatus.dwCurrentState == SERVICE_RUNNING)
		OutputDebugStringW(L"\nController Service started.\n");
	else
		OutputDebugStringW(L"\nFailed to start Controller Service.\n");
}

//安装服务
void CmdInstallService()
{
	SC_HANDLE   schService;
	SC_HANDLE   schSCManager;
	WCHAR		szPath[MAX_PATH];
	WCHAR		szQuotePath[MAX_PATH + 1];

	SERVICE_DESCRIPTION szDiscription;

	if (GetModuleFileName(NULL, szPath, MAX_PATH) == 0)
	{
		OutputDebugStringW(L"Unable to install Controller Service.\n");
		return;
	}
	wsprintf(szQuotePath, L"\"%s\"", szPath);

	schSCManager = OpenSCManager(
		NULL,                   // machine (NULL == local)
		NULL,                   // database (NULL == default)
		SC_MANAGER_ALL_ACCESS   // access required
	);
	if (schSCManager)
	{
		schService = CreateService(
			schSCManager,               // SCManager database
			SZ_SERVICE_NAME,			// name of service
			SZ_SERVICE_DISPLAY_NAME,	// name to display
			SERVICE_ALL_ACCESS,         // desired access
			SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,       // error control type
			szQuotePath,                // service's binary
			NULL,                       // no load ordering group
			NULL,                       // no tag identifier
			NULL/*SZ_DEPENDS_SERVICE_NAME*/,	// dependencies
			NULL,                       // LocalSystem account
			NULL);                      // no password

		if (schService)
		{
			szDiscription.lpDescription =(LPWSTR) SZ_SERVICE_DISPLAY_NAME;
			ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &szDiscription);

			OutputDebugStringW(L"Controller Service installed.\n");
			CmdStartService(schService);
			CloseServiceHandle(schService);
		}
		else
		{
			OutputDebugStringW(L"Installing Controller Service failed.\n");
		}

		CloseServiceHandle(schSCManager);
	}
	else
		OutputDebugStringW(L"Open SCManager failed.\n");
}


//服务入口函数									
VOID WINAPI ServiceMain(DWORD dwArgc, LPCWSTR *lpszArgv)
{
	// register our service control handler:
	sshStatusHandle = RegisterServiceCtrlHandler(L"SZ_SERVICE_NAME", ServiceCtrl);

	ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ssStatus.dwServiceSpecificExitCode = 0;

	// report the status to the service control manager.
	if (!ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 3000))
	{
		if (sshStatusHandle)
			(VOID)ReportStatusToSCMgr(SERVICE_STOPPED, dwErr, 0);
	}

	ServiceStart(dwArgc, lpszArgv);

	// try to report the stopped status to the service control manager.
	if (sshStatusHandle)
		(VOID)ReportStatusToSCMgr(SERVICE_STOPPED, dwErr, 0);
}

VOID WINAPI ServiceCtrl(DWORD dwCtrlCode)
{
	// Handle the requested control code.
	switch (dwCtrlCode)
	{

		/* 停止服务时，需要所有的沙化进程全部结束 */
	case SERVICE_CONTROL_STOP:
	{
		ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
		ServiceStop();
	}
	return;

	// Update the service status.
	case SERVICE_CONTROL_INTERROGATE:
		break;

		// invalid control code
		//
	default:
		break;
	}
	ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
}
// 向 SCM 报告服务状态
BOOL ReportStatusToSCMgr(DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;
	BOOL fResult = TRUE;

	if (dwCurrentState == SERVICE_START_PENDING)
		ssStatus.dwControlsAccepted = 0;
	else
		ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	ssStatus.dwCurrentState = dwCurrentState;
	ssStatus.dwWin32ExitCode = dwWin32ExitCode;
	ssStatus.dwWaitHint = dwWaitHint;

	if ((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED))
		ssStatus.dwCheckPoint = 0;
	else
		ssStatus.dwCheckPoint = dwCheckPoint++;

	// Report the status of the service to the service control manager.
	fResult = SetServiceStatus(sshStatusHandle, &ssStatus);

	return fResult;
}

//服务实际功能执行区
VOID ServiceStart(DWORD dwArgc, LPCWSTR *lpszArgv)
{
	// create the event object. The control handler function signals
	// this event when it receives the "stop" control code.

	hServerStopEvent = CreateEvent(
		NULL,    // no security attributes
		TRUE,    // manual reset event
		FALSE,   // not-signalled
		NULL);   // no name

				 // report the status to the service control manager.
	if (!ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0))
	{
		if (hServerStopEvent)
			CloseHandle(hServerStopEvent);
		return;
	}

	BOOL bRet = FALSE;
	bRet = PreRun();

	WaitForSingleObject(hServerStopEvent, INFINITE);

	//cleanup:
	if (hServerStopEvent)
		CloseHandle(hServerStopEvent);
}

//停止服务
VOID ServiceStop()
{
	if (hServerStopEvent)
	{
		KillProcessByName(L"twaina.exe");
		SetEvent(hServerStopEvent);
	}
}
//执行服务需要做的事情
bool PreRun()
{

}
static BOOL KillProcessByName(const TCHAR *lpszProcessName) {
	unsigned int   pid = -1;
	BOOL    retval = TRUE;

	if (lpszProcessName == NULL)
		return -1;

	DWORD dwRet = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(PROCESSENTRY32);
	int flag = Process32First(hSnapshot, &processInfo);

	// Find the process with name as same as lpszProcessName  
	while (flag != 0)
	{
		if (_tcscmp(processInfo.szExeFile, lpszProcessName) == 0) {
			// Terminate the process.  
			pid = processInfo.th32ProcessID;
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);

			if (TerminateProcess(hProcess, 0) != TRUE) { // Failed to terminate it.  
				retval = FALSE;
				break;
			}
		}

		flag = Process32Next(hSnapshot, &processInfo);
	} // while (flag != 0)  

	CloseHandle(hSnapshot);

	if (pid == -1)
		return FALSE;

	return retval;
}