#include "stdafx.h"
#include<iostream>
#include "UnitTest.h"
#include "DumpFile.h"
//class Singleton {
//private:
//	Singleton();
//	Singleton(const Singleton&);
//	Singleton& operator=(const Singleton&);
//public:
//	static Singleton& getInstance();
//};
//Singleton& Singleton::getInstance()
//{
//	static Singleton instance;
//	return instance;
//}
class Singleton {
private:
	Singleton();
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);
public:
	static Singleton& getInstance()
	{
		static Singleton instance;
		return instance;
	}
};
unsigned int __stdcall threadDemo(LPVOID) // void *
{
	Singleton::getInstance();
	return 0;
}
#include<mutex>
std::mutex mut;
int main()
{
	mut.lock();

	mut.unlock();
	cir
	for (int i = 0; i < 10; i++)
	{
		HANDLE  handle = (HANDLE)_beginthreadex(nullptr, 0, threadDemo, nullptr, 0, nullptr);
		if (handle != 0)
		{
			::WaitForSingleObject(handle, INFINITE);
		}
	}
	//SetUnhandledExceptionFilter(UnhandledExceptionProc);
	/*LogInfo("Main test");
	IOCPTest();
	system("pause");*/
	return 0;
}

