#include "windows.h"
#include "stdio.h"

HANDLE map, sema;

int main()
{
	system("cls");
	sema = OpenSemaphore(SYNCHRONIZE, FALSE, "sema1");
	WaitForSingleObject(sema, INFINITE);
	map = OpenFileMapping(FILE_MAP_READ, FALSE, "map");
	char* buf = (char*)MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
	printf("prochteno iz %08X - %s", &buf, buf);
	getchar();
	return 0;
}