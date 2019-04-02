#include "windows.h"
#include "stdio.h"

HANDLE map, sema;

int main()
{
	system("cls");
	sema = CreateSemaphore(NULL, 0, 1, "sema1");
	map = CreateFileMapping((HANDLE)-1, NULL, PAGE_EXECUTE_READWRITE, 0, 1000, "map");
	char* buf = (char*)MapViewOfFile(map, FILE_MAP_WRITE, 0, 0, 0);
	Sleep(2000);
	sprintf(buf,"one message");
	printf ("Zapisano v %08X - %s\n", &buf, buf);
	ReleaseSemaphore(sema, 1, NULL);
	
	char *buf1 = (char*)VirtualAlloc(NULL, 1000, MEM_COMMIT, PAGE_READWRITE);
	for (int i=0; i<26; i++) {
		*(buf1+399*i) = 'a'+i;
		printf("%08X = %c\n",buf1+399*i, *(buf1+399*i));
	}
	getchar();
	CloseHandle(sema);
	UnmapViewOfFile(buf);
	CloseHandle(map);
	return 0;

}