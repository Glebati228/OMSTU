#include <windows.h>
#include <stdio.h> 
int main(int argc, char* argv) {
	int k; 
	BOOL rc, rc2;
    HANDLE jb, jb2;		
	STARTUPINFO si, si2;
	PROCESS_INFORMATION pi, pi2;  
   memset(&si, 0, sizeof(STARTUPINFO));  
   si.cb=sizeof(si);   
   rc=CreateProcess(NULL, "child.exe", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
    memset(&si2, 0, sizeof(STARTUPINFO));  
   si2.cb=sizeof(si2);  
   rc2=CreateProcess(NULL, "child2.exe", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si2, &pi2);
   
    jb = CreateJobObject(NULL, NULL);
    if(jb == INVALID_HANDLE_VALUE) {
	    printf("Error code: %ld\n", GetLastError());
    }
    if(!AssignProcessToJobObject(jb, pi.hProcess)){
	   printf("Error code: %ld\n", GetLastError());
    }
    if (!rc)    {
	    printf("Error create Process, codeError = %ld\n", GetLastError()); 
	    getchar(); 
	    return 0; 
	    }   
		
	   printf("For Child Process:\n"); 
	   printf("hProcess=%d  hThread=%d  ProcessId=%ld  ThreadId=%ld\n", pi.hProcess, pi.hThread, pi.dwProcessId, pi.dwThreadId);  
	   
	   jb2 = CreateJobObject(NULL, NULL);
    if(jb2 == INVALID_HANDLE_VALUE) {
	    printf("Error code2: %ld\n", GetLastError());
    }
    if(!AssignProcessToJobObject(jb2, pi2.hProcess)){
	   printf("Error code2: %ld\n", GetLastError());
    }
    if (!rc2)    {
	    printf("Error create Process2, codeError = %ld\n", GetLastError()); 
	    getchar(); 
	    return 0; 
	    }  
		
	   printf("For Child Process:\n"); 
	   printf("hProcess=%d  hThread=%d  ProcessId=%ld  ThreadId=%ld\n", pi2.hProcess, pi2.hThread, pi2.dwProcessId, pi2.dwThreadId);
	   
	   for (k = 0; k < 15; k++)       
	   {
		   printf("I am Parent... (my K=%d)\n", k); 
		   Sleep(1000);   
		   if(k == 7){
			    printf("Terminate child\n"); 
           TerminateJobObject(jb, 0);	
		   }
		   if(k == 11){
		   printf("Terminate child2\n"); 
           TerminateJobObject(jb2, 0);	
		   break;
		   }
		   
		   }
			   
		   CloseHandle(pi.hProcess);   
		   CloseHandle(pi.hThread);  
		    CloseHandle(pi2.hProcess);   
		   CloseHandle(pi2.hThread);
		   return 0;
}