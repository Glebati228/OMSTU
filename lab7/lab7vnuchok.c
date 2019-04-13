#include <stdio.h> 
#include <windows.h>
int main() 
{
	int k;   
	printf("Demonstration processes, Child Proccess\n");  
	for (k=0; k<30; k++)   
		{
			printf("I am Vnuchok ... (k=%d)\n\r", k);    
			
	Sleep(500); 
	} 
	return 0; 
	} 