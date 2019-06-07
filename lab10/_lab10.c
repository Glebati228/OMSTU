#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <unistd.h>

void main()
{
	char *pb, st[20];
	int hmem;
	int k;
	sleep(4);
	hmem=shmget(18011970, 16000, 0600);
			if (hmem == -1)
			{
				printf("Error Shared Get Mem with\n"); 
				getchar(); 
				exit(0);
			}
			pb=shmat(hmem, NULL, SHM_RDONLY);
			if (pb == 0)
			{
				printf("Error Attach Shared Mem\n"); 
				getchar(); 
				exit(0);
			}
			for (k = 0; k < 10; k++)
			{
				//pthread_mutex_lock(&mtx);
				strncpy(st, pb, 20); 
				st[19]='\0'; 
				printf("%s\n", st);
				//pthread_mutex_unlock(&mtx); 
				sleep(2);
			}
			shmdt(pb);
}
