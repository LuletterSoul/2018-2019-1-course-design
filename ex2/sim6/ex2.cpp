// sm6.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <time.h>

#define INTE_PER_SEC  1000
#define MAX_THREAD_NUM  64
#define MAX_EVENT_NUM 64

struct ThreadInfo
{
	int	serial;
	double	delay;
};

volatile  int accnt1 = 0; /*  in the memory */
volatile  int accnt2 = 0;
volatile  int accnt = 0;
volatile  int rand_num = 10;


void account(const char* file);
void acc(void* p);

////////////////////////////////////////////////////////
// main fuction
////////////////////////////////////////////////////////



int _tmain(int argc, _TCHAR* argv[])
{
	char ch;

	while (TRUE)
	{
		const char *file = "sm6.dat";
		// Cleare screen
		system("cls");

		// display prompt info
		printf("*********************************************\n");
		printf("       1.Start test\n");
		printf("       2.Exit to Windows\n");
		printf("*********************************************\n");
		printf("Input your choice(1or2): ");

		// if the number inputed is error, retry!
		do {
			ch = (char)_getch();
		} while (ch != '1' && ch != '2');

		system("cls");
		if (ch == '1')
			account(file);
		else if (ch == '2')
			return 0;
		printf("\nPress any key to finish this Program. \nThank you test this Proggram!\n");
		_getch();
	} //end while
} //end main

void account(const char* file)
{
	DWORD n_thread = 0, n_event=0;
	DWORD thread_ID;
	DWORD wait_for_all;

	// Tread Object Array

	HANDLE h_Thread[MAX_THREAD_NUM],h_Events[MAX_EVENT_NUM];
	ThreadInfo  thread_info[MAX_THREAD_NUM];

	LPSECURITY_ATTRIBUTES lpEventAttributes = NULL; // pointer to security attributes
	BOOL bManualReset = FALSE;  // flag for manual-reset event
	BOOL bInitialState = FALSE; // flag for initial state
	LPCTSTR lpName1 = TEXT("cacuEvent");      // pointer to event-object name
	LPCTSTR lpName2 = TEXT("validateEvent"); // pointer to event-object name
	LPCTSTR lpName3 = TEXT("completeEvent"); // pointer to event-object name

	//用以控制加减顺序 
	h_Events[n_event++] = CreateEvent(
		lpEventAttributes,			// pointer to security attributes
		bManualReset,				// flag for manual-reset event
		bInitialState,				// flag for initial state
		lpName1						// pointer to event-object name
	);


	//用以指示检验
	h_Events[n_event++] = CreateEvent(
		lpEventAttributes,			// pointer to security attributes
		bManualReset,				// flag for manual-reset event
		bInitialState,				// flag for initial state
		lpName2						// pointer to event-object name
	);

	//报告检验完成,初始化有信号是为了让计算线程可以被唤醒
	h_Events[n_event++] = CreateEvent(
		lpEventAttributes,			// pointer to security attributes
		bManualReset,				// flag for manual-reset event
		TRUE,				// flag for initial state
		lpName3						// pointer to event-object name
	);

	//生成随机数
	srand((unsigned)time(NULL));
	rand_num = rand() % 1000;

	std::ifstream  inFile;
	inFile.open(file);		//open file
	printf("Now, We begin to read thread Information to thread_info array \n\n");
	while (inFile)
	{
		// read every thread info
		inFile >> thread_info[n_thread].serial;
		inFile >> thread_info[n_thread++].delay;
		inFile.get();
	} //end while

	  // Create all thread
	for (int i = 0; i < (int)(n_thread); i++)
	{
		// Create a thread
		h_Thread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(acc), &thread_info[i], 0, &thread_ID);
	} //end for
	  // Create thread

	  // waiting all thread will been finished

	wait_for_all = WaitForMultipleObjects(n_thread, h_Thread, TRUE, -1);

	//回收资源
	for (int i = 0; i < (int)(n_event); i++)
	{
		CloseHandle(h_Events[i]);
	} //end for

	printf("All threads have finished Operating.\n");
}// end account

void acc(void* p)
{
	DWORD m_delay;
	int m_serial;

	DWORD dwDesiredAccess = EVENT_ALL_ACCESS;	// access flag
	BOOL bInheritHandle = FALSE;				// inherit flag
	int counter = 0;;

	LPCTSTR lpName1 = TEXT("cacuEvent");      // pointer to event-object name
	LPCTSTR lpName2 = TEXT("validateEvent"); // pointer to event-object name
	LPCTSTR lpName3 = TEXT("completeEvent"); // pointer to event-object name
	HANDLE h_Events[MAX_EVENT_NUM];

	h_Events[0]=OpenEvent(
								dwDesiredAccess,  // access flag
								bInheritHandle,    // inherit flag
								lpName1         // pointer to event-object name
);	

	h_Events[1] = OpenEvent(
		dwDesiredAccess,  // access flag
		bInheritHandle,    // inherit flag
		lpName2         // pointer to event-object name
	);

	h_Events[2] = OpenEvent(
		dwDesiredAccess,  // access flag
		bInheritHandle,    // inherit flag
		lpName3         // pointer to event-object name
	);


	m_serial = ((ThreadInfo*)(p))->serial;
	m_delay = (DWORD)(((ThreadInfo*)(p))->delay*INTE_PER_SEC);
	srand((unsigned)((ThreadInfo*)(p))->delay);

	do {
			//线程1负责减操作 
			if (m_serial == 1) {
			
				WaitForSingleObject(h_Events[2], -1);//等待检验完毕
				printf("I am thread  %d , I am subscribing ramdom num %d \n", m_serial, rand_num);
				accnt1 = accnt1 - rand_num;
				printf("I am thread  %d , I have finished sub operation.Accnt1 value is %d\n", m_serial, accnt1);
				Sleep(m_delay);
				SetEvent(h_Events[0]);
			}
			//线程2负责加操作 
			else if (m_serial == 2) {
				//等待线程1完成减操作
				WaitForSingleObject(h_Events[0], -1);//当且仅当随机数已产生，1线程已执行减操作
				printf("I am thread  %d , I am adding ramdom num %d \n", m_serial, rand_num);
				accnt2 = accnt2 + rand_num;
				printf("I am thread  %d , I have finished adding operation.Accnt2 value is %d\n", m_serial, accnt2);
				SetEvent(h_Events[1]);
			}
			//线程3负责检验 
			else if (m_serial == 3) {
				//等待线程2报告完成了加减操作 
				WaitForSingleObject(h_Events[1], -1);
				accnt = accnt1 + accnt2;
				printf("I am thread  %d , I am validating the result.Result is %d\n\n", m_serial, accnt);
				SetEvent(h_Events[2]);
				Sleep(m_delay);
		}
			//critical_section  end
			counter++;
		} while ((accnt == 0) && (counter<10));
	printf("Now accnt1+accnt2 =  %05d\n", accnt);
} //end acc


