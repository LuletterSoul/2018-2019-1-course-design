#pragma once
#include <windows.h>

class Semaphore
{
public:
	Semaphore();
	Semaphore(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
		LONG                  lInitialCount,
		LONG                  lMaximumCount,
		LPCWSTR                lpName);
	Semaphore(
		LONG                  lInitialCount,
		LONG                  lMaximumCount,
		LPCWSTR                lpName);
	Semaphore(LONG lMaximumCount);
	~Semaphore();
	DWORD wait(DWORD dwMilliseconds); //P操作，支持有限等待
	DWORD wait();					  //P操作，默认无限等待
	DWORD signal(LONG lReleaseCount); //V操作，支持指定释放数量
	DWORD signal();					  //V操作，默认释放1
private:
	HANDLE h_semaphore;				  //信号量执行对象
};

