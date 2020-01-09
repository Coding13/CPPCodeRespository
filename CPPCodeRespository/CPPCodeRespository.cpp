#include "stdafx.h"
#include "UnitTest.h"
#include "DumpFile.h"

int main()
{
	SetUnhandledExceptionFilter(UnhandledExceptionProc);
	LogInfo("Main test");
	IOCPTest();
	system("pause");
	return 0;
}

