#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
using namespace std;
#define MAX_LINE_LEN  80

int main(int argc,char *argv[]) {
//	argc = 4;
//	argv[1] = "c:\\windows\\notepad.exe";
//	argv[2] = "c:\\windows\\system32\\calc.exe";
//	argv[3] = "c:\\windows\\write.exe";
	/* Creat a new process to execute */
	char cmdLine[MAX_LINE_LEN];
	int i;

	/* CreateProcess parameters */
	LPCTSTR lpApplicationName = NULL;
	LPSECURITY_ATTRIBUTES processSA = NULL;		/* Default */
	LPSECURITY_ATTRIBUTES threadSA = NULL;		/* Default */
	BOOL shareRights = TRUE;					/* Default */	
	DWORD creationMask =	CREATE_NEW_CONSOLE;	/* Window per process */
	LPVOID environment = NULL;					/* Default */
	LPTSTR curDir = NULL;						/* Default */
	
	STARTUPINFO startInfo;						/* Result */	
	PROCESS_INFORMATION processInfo;			/* Result */

	ZeroMemory(&startInfo,sizeof(startInfo));
	startInfo.cb = sizeof(startInfo);

	/* 1.Read the command line parameters */
	if(!(argc >= 2)) {
		fprintf(stderr,"Usage: launch <launch_set_filename>\n");
		exit(0);
	}
	
	 ifstream infile;
	 /* 2.read the file name from command line parameters[1].Commonly regard it as the absolute path.*/
	 char * input_file =argv[1];
	 /* 3.Open file.Print error to standart output when failure occrrus.*/
	 infile.open(input_file,ifstream::in);
	 if (infile.fail()) {
	 	fprintf(stderr,"Could not open file.\n");
	 }
	 char line[MAX_LINE_LEN];
	 int r = 0;
	/* 3.read the command from file that included excutable commands */
	 while(infile >> line){
	 	strcpy(cmdLine,line);
	/* 4.create process per command line from file.*/
	 	if(!CreateProcess(	
									    lpApplicationName,	/* File name of executable */
										cmdLine,		/* Command line */
										processSA,			/* Process inherited security */
										threadSA,			/* Thread inherited security */
										shareRights,		/* Right propagation */
										creationMask,		/* Various creation flags */
										environment,		/* Environment variabkesr */
										curDir,				/* Child's current directory */
										&startInfo,
										&processInfo
									 )
					  ) {
							fprintf(stderr,"CreatProcess failed on error %d\n",GetLastError());
							ExitProcess(1);
						};
					fprintf(stdout,"The Child Process's PID: %d.\n", processInfo.dwProcessId);
					fprintf(stdout,"The Parent Process finish.\n");
					Sleep(500);
					CloseHandle(processInfo.hProcess);
					CloseHandle(processInfo.hThread);			
	 }
	Sleep(5000);
	return (0);
}
