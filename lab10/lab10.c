#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>

void main()
{
	int pid;
	pthread_mutex_t mtx;
	int shmem;
	char *pm;
	pid = fork();
	if(pid == -1)
	{
		perror("error creating a fork");
		exit(1);
	}
	if(!pid)
	{
		execl("l.out", 0);
	}
		//	pthread_mutex_lock(&mtx);	
			shmem = shmget(18011970, 16000, IPC_CREAT | 0600);
			if(shmem == -1)
			{
				perror("Error creating shared memory: ");
				getchar();
				exit(0);
			}
			pm = shmat(shmem, NULL, 0);
			if(pm == NULL)
			{
				perror("shmat");
				getchar();
				exit(3);
			}
			printf("writing to shared memory\n");
			strcpy(pm, "salem");
		//	pthread_mutex_unlock(&mtx);
			sleep(8);	
			shmdt(pm);
			shmctl(shmem, IPC_RMID, NULL);
} 
