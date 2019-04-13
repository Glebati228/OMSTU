#include <stdio.h> 
#include <windows.h>
int main(int argc, char* argv) 
{
	int k; 
	BOOL rc;
    HANDLE jb;	
	STARTUPINFO si;
    	
	PROCESS_INFORMATION pi;  
   memset(&si, 0, sizeof(STARTUPINFO));  
   si.cb=sizeof(si);   
   rc=CreateProcess(NULL, "vnuchok.exe", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
   // jb = CreateJobObject(NULL, NULL);
   // if(jb == INVALID_HANDLE_VALUE) {
	   // printf("Error code: %ld\n", GetLastError());
   // }
   // if(!AssignProcessToJobObject(jb, pi.hProcess)){
	  // printf("Error code: %ld\n", GetLastError());
   // }
   // if (!rc)    {
	   // printf("Error create Process, codeError = %ld\n", GetLastError()); 
	   // getchar(); 
	   // return 0; 
	   // }   
	   printf("For Child Process:\n"); 
	   printf("hProcess=%d  hThread=%d  ProcessId=%ld  ThreadId=%ld\n", pi.hProcess, pi.hThread, pi.dwProcessId, pi.dwThreadId);  
	   for (k = 0; k < 30; k++)       
	   {
		   printf("I am Child ... (k=%d)\n\r", k);
		   Sleep(500);   
		   }
           //TerminateJobObject(jb, 0);		   
		   CloseHandle(pi.hProcess);   
		   CloseHandle(pi.hThread);  
		   return 0;
}
	
	
 