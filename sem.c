#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <pthread.h>
/*Implementation of semaphores using mutexes and conditional variables*/

int critical=0;


pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
int nr=3;//initialise semaphore to this value

void P(){
  pthread_mutex_lock(&mutex);
  while(nr==0){
    pthread_cond_wait(&cond,&mutex);
  }

  nr--;
  pthread_mutex_unlock(&mutex);
}

void V(){
  pthread_mutex_lock(&mutex);
  nr++;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
}

void* example(void* arg){
  P();
  critical++;

  usleep(100000);
  printf("Number of threads in critical region is %d\n",critical);

  critical--;
  V();
}



int main(){
  int i=0;
  pthread_t t[10];

  for(i=0;i<10;i++){
    pthread_create(&t[i],NULL,example,NULL);
  }
  for(i=0;i<10;i++){
    pthread_join(t[i],NULL);
  }
	return 0;
}
