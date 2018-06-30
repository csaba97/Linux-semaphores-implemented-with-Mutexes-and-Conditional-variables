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


#define MAX_IN_CRIT_REGION 3
int critical=0;
int sem_id;

pthread_mutex_t mutex[100];
pthread_cond_t cond[100];
int nrSem=0;
int semValues[100];


/*IMPORTANT
initSem() function is not sincronyzed itself so it should not be called in threads etc.
*/
int initSem(int initValue){
	if(nrSem>=99){
		printf("Cannot create more semaphores\n");
		return -1;
	}
	nrSem++;
	pthread_mutex_init(&mutex[nrSem],NULL);
	pthread_cond_init(&cond[nrSem],NULL);
	semValues[nrSem]=initValue;
	return nrSem;
}

void P(int semId){
	pthread_mutex_lock(&mutex[semId]);
	while(semValues[semId]==0){
		pthread_cond_wait(&cond[semId],&mutex[semId]);
	}

	semValues[semId]--;
	pthread_mutex_unlock(&mutex[semId]);
}

void V(int semId){
	pthread_mutex_lock(&mutex[semId]);
	semValues[semId]++;
	pthread_cond_signal(&cond[semId]);
	pthread_mutex_unlock(&mutex[semId]);
}

void* example(void* arg){
	P(sem_id);
	critical++;

	int r=rand()%10;
	usleep(100000 *r);
	printf("Number of threads in critical region is %d\n",critical);

	critical--;
	V(sem_id);
}



int main(){
	int i=0;
	pthread_t t[10];
	sem_id=initSem(MAX_IN_CRIT_REGION);
	srand(time(NULL));

	for(i=0;i<10;i++)
	pthread_create(&t[i],NULL,example,NULL);

	for(i=0;i<10;i++)
	pthread_join(t[i],NULL);

	return 0;
}
