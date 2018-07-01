#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define MAX_IN_CRIT_REGION 4
#define MAX_PIPES 100
#define READ_END 0
#define WRITE_END 1


int critical,sem_id,nrSem=0,fd[MAX_PIPES+1][2];

/*IMPORTANT
initSem() function is not sincronyzed itself so it should not be called in threads etc.
*/
int initSem(int initValue){
	int i;
	if(nrSem>=MAX_PIPES){
		printf("Cannot create more semaphores\n");
		return -1;
	}
	nrSem++;
	pipe(fd[nrSem]);
	for(i=1;i<=initValue;i++)
	write(fd[nrSem][WRITE_END],"a",1);
	return nrSem;
}

void P(int semId)
{
	char c;
	read(fd[semId][READ_END],&c,1);
}

void V(int semId)
{
	write(fd[semId][WRITE_END],"a",1);
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





int main(int argc, char *argv[])
{
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
