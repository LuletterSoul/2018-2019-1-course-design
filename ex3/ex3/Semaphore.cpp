#include "stdafx.h"
#include "Semaphore.h"



Semaphore::Semaphore(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
	LONG                  lInitialCount,
	LONG                  lMaximumCount,
	LPCWSTR                lpName) {
	this->h_semaphore = CreateSemaphore(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);
	if (this->h_semaphore == NULL) {
		fprintf(stderr, "Create Semaphore Error in %d.", GetLastError());
	}
}

Semaphore::Semaphore(
	LONG                  lInitialCount,
	LONG                  lMaximumCount,
	LPCWSTR                lpName) {
	this->h_semaphore = CreateSemaphore(NULL,lInitialCount,lMaximumCount,lpName);
	if (this->h_semaphore == NULL) {
		fprintf(stderr, "Create Semaphore Error in %d.", GetLastError());
	}
}

Semaphore::Semaphore()
{
	this->h_semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	if (this->h_semaphore == NULL) {
		fprintf(stderr, "Create Semaphore Error in %d.", GetLastError());
	}
}

Semaphore::Semaphore(LONG lMaximumCount)
{
	this->h_semaphore = CreateSemaphore(NULL, lMaximumCount, lMaximumCount, NULL);
	if (this->h_semaphore == NULL) {
		fprintf(stderr, "Create Semaphore Error in %d.", GetLastError());
	}
}


Semaphore::~Semaphore()
{
	CloseHandle(this->h_semaphore);
}

DWORD Semaphore::wait() {
	return wait(INFINITE);
}

DWORD Semaphore::wait(DWORD dwMilliseconds = INFINITE) {
	return WaitForSingleObject(this->h_semaphore, dwMilliseconds);
}

DWORD Semaphore::signal() {
	return signal(1);
}

DWORD Semaphore::signal(LONG lReleaseCount = 1) {
	return ReleaseSemaphore(this->h_semaphore, lReleaseCount, NULL);
}
