#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

char text[] = "qwertyuiopasdfghjklzxcvbnm";
pthread_t tid3;
pthread_mutex_t mtx;

void pthread3(void* arg){
  int k = arg;
  pthread_mutex_lock(&mtx);
  if(k == 1 || k == 3)
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  for(int i = 0; i < 20; i++){
    printf("\033[%d;%dH\033[%dm", i+1, (k == 1) ? 20 : (k == 2) ? 40 : 60, (k == 1) ? 34 : (k == 2) ? 32 : 31);
    if(i == 13 && k == 3){
      pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
      pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    }
    if(i == 17 && k == 3) {
      printf("\033[%d;1H\033[37m thread 3 term", i+1);
      pthread_testcancel();
    }
    for(int j = 0; j < (int)arg * 2; j++)
      printf("%c", text[i]);
    printf("\n");
    pthread_mutex_unlock(&mtx);
    sleep(1);
  }
}

void main(){
int rc;
printf("\033[2J\n");
for(int i = 1; i < 4; i++)
pthread_create(&tid3, NULL, (void*)pthread3, (void*)i);


for(int i = 0; i < 20; i++){
printf("\033[%d;1H\033[37m", i+1);
printf("%c\n", text[i]);
if(i == 6){
pthread_cancel(tid3);
printf("\033[0m try to term 1 thread\n");
}
if(i==11){
pthread_cancel(tid3);
printf("\033[0m try to term 3 thread\n");
}

sleep(1);
}

printf("\033[0m");
getchar();
}
