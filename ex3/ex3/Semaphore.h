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
	DWORD wait(DWORD dwMilliseconds); //P������֧�����޵ȴ�
	DWORD wait();					  //P������Ĭ�����޵ȴ�
	DWORD signal(LONG lReleaseCount); //V������֧��ָ���ͷ�����
	DWORD signal();					  //V������Ĭ���ͷ�1
private:
	HANDLE h_semaphore;				  //�ź���ִ�ж���
};

