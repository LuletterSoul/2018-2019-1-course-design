// ex3.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <Semaphore.h>

#define INTE_PER_SEC  1000
#define MAX_THREAD_NUM  64
#define MAX_EVENT_NUM 64
#define MAX_BUF_SIZE 1024

struct ThreadInfo
{
	int	serial;
	double	delay;
	double  sleep;
};
Semaphore * read_mutex;//读者信号量
Semaphore * write_mutex;//写者信号量
Semaphore * mutex = new Semaphore(1);
volatile int r_count = 0;
char buf[MAX_BUF_SIZE];

void init(const char* p_file, const char * c_file);
void init_thread(const char * p_file, ThreadInfo * p_thread_info, LPTHREAD_START_ROUTINE routine, DWORD &n_thread, HANDLE * p_Thread, Semaphore *& sema);
void writer_routine(void *p);
void reader_routine(void *p);

////////////////////////////////////////////////////////
// main fuction
////////////////////////////////////////////////////////



int _tmain(int argc, _TCHAR* argv[])
{
	char ch;

	while (TRUE)
	{
		const char *p_file = "writers.dat";//写者定义
		const char *c_file = "readers.dat";//读者定义
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
		if (ch == '1') {
			init(p_file,c_file);	
		}
		else if (ch == '2')
			return 0;
		printf("\nPress any key to finish this Program. \nThank you test this Proggram!\n");
		_getch();
	} //end while
} //end main



void init(const char* p_file,const char * c_file)
{
	DWORD n_pthread = 0, n_cthread = 0,n_event = 0;
	//DWORD thread_ID;
	DWORD wait_for_all;

	// Tread Object Array
	HANDLE p_Thread[MAX_THREAD_NUM],c_Thread[MAX_THREAD_NUM];
	ThreadInfo  w_thread_info[MAX_THREAD_NUM], r_thread_info[MAX_THREAD_NUM];

	LPSECURITY_ATTRIBUTES lpEventAttributes = NULL; // pointer to security attributes
	BOOL bManualReset = FALSE;  // flag for manual-reset event
	BOOL bInitialState = FALSE; // flag for initial state

	//初始化缓冲区
	memset(buf, '\0', MAX_BUF_SIZE);
	strcpy_s(buf, "Empty.");
	//初始化写者线程
	init_thread(p_file, w_thread_info,(LPTHREAD_START_ROUTINE) writer_routine, n_pthread, p_Thread, write_mutex);
	//初始化读者线程
	init_thread(c_file, r_thread_info,(LPTHREAD_START_ROUTINE)reader_routine, n_cthread, c_Thread, read_mutex);

	// waiting all thread will been finished
	wait_for_all = WaitForMultipleObjects(n_pthread, p_Thread, TRUE, -1);
	wait_for_all = WaitForMultipleObjects(n_cthread, c_Thread, TRUE, -1);
	delete write_mutex;
	delete read_mutex;
	delete mutex;
	printf("All threads have finished Operating.\n");
}
void init_thread(const char * file, ThreadInfo * thread_info, LPTHREAD_START_ROUTINE routine, DWORD &n_thread, HANDLE * h_thread,Semaphore *& sema)
{
	std::ifstream  inFile;
	inFile.open(file);		//open file
	//printf("Now, We begin to read thread Information to thread_info array \n\n");
	while (inFile)
	{
		// read every thread info
		inFile >> thread_info[n_thread].serial;
		inFile >> thread_info[n_thread].delay;
		inFile >> thread_info[n_thread++].sleep;
		inFile.get();
	} //end while
	sema = new Semaphore(n_thread);//初始化信号量
	  // Create all thread
	for (int i = 0; i < (int)(n_thread); i++)
	{
		// Create a thread
		h_thread[i] = CreateThread(NULL, 0, routine, &thread_info[i], 0, NULL);
	} //end for
}
// end account

void writer_routine(void *p) 
{
	DWORD m_delay,m_sleep;
	int m_serial;
	m_serial = ((ThreadInfo*)(p))->serial;
	m_delay = (DWORD)(((ThreadInfo*)(p))->delay*INTE_PER_SEC);
	m_sleep = (DWORD)(((ThreadInfo*)(p))->sleep*INTE_PER_SEC);
	Sleep(m_delay);
	write_mutex->wait();
	DWORD feedback=  mutex->wait();
	if (feedback == WAIT_FAILED) {
		fprintf(stderr, "Wait critical section error in %d.\n", GetLastError());
	}
	printf("★ I am writer %d.I am writing the content.Please wait........\n", m_serial);
	sprintf_s(buf, "★ Hello,I am writer %d.\n", m_serial);//将写者内容写进缓冲区
	Sleep(m_sleep);										  //模拟写入时间
	printf("★ Writer %d write complete.\n\n",m_serial);
	mutex->signal();
	write_mutex->signal();
}
void reader_routine(void *p) 
{
	DWORD m_delay, m_sleep;
	int m_serial;
	m_serial = ((ThreadInfo*)(p))->serial;
	m_delay = (DWORD)(((ThreadInfo*)(p))->delay*INTE_PER_SEC);
	m_sleep = (DWORD)(((ThreadInfo*)(p))->sleep*INTE_PER_SEC);
	//printf("I am reader %d\n,will delay %d,sleep %d\n", m_serial, m_delay, m_sleep);
	Sleep(m_delay);
	read_mutex->wait();
	if (r_count == 0)
		mutex->wait();  //如果当前没有读者，第一个读者锁定临界区
	++r_count;
	read_mutex->signal();
	printf("● I am reader %d.I am reading the content.Please wait........\n", m_serial);
	Sleep(m_sleep);
	printf("● Reader %d read content from buf:%s\n", m_serial, buf); //读者读出缓冲区内容
	printf("● Reader %d read complete.\n\n",m_serial);
	read_mutex->wait();
	--r_count;
	if (r_count == 0)
		mutex->signal();//读者完成读后，释放临界区
	read_mutex->signal();
}



