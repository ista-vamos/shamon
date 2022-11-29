#include <threads.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

sem_t lock1, lock2, rlock;
uint64_t countA, countB, countA1, countB1, countA2, countB2;
int running=1;
int Thread1(void* arg)
{
	int r;
	while(running)
	{
		sem_wait(&rlock);
		r=rand()%2;
		sem_post(&rlock);
		if(r%2)
		{
			sem_wait(&lock1);
			countA++;
			sem_post(&lock1);
			countA1++;
		}
		else
		{
			sem_wait(&lock2);
			countB++;
			sem_post(&lock2);
			countB1++;
		}
	}
	return 0;
}
int Thread2(void* arg)
{
	int r;
	while(running)
	{
		sem_wait(&rlock);
		r=rand()%2;
		sem_post(&rlock);
		if(r%2)
		{
			sem_wait(&lock1);
			countA++;
			sem_post(&lock1);
			countA2++;
		}
		else
		{
			sem_wait(&lock2);
			countB++;
			sem_post(&lock2);
			countB2++;
		}
	}
	return 0;
}

int main(int argc, char**argv)
{
	char *line;
	srand(time(NULL));
	size_t linelen;
	thrd_t thread1;
	thrd_t thread2;
	countA=0;
	countB=0;
	countA1=0;
	countB1=0;
	countA2=0;
	countB2=0;
	sem_init(&rlock,0,1);
	sem_init(&lock1,0,1);
	sem_init(&lock2,0,1);
	thrd_create(&thread1,Thread1,0);
	thrd_create(&thread2,Thread2,0);
	while(getline(&line, &linelen, stdin)>=0)
	{
		if(strncmp(line,"safe",4)==0)
		{
			sem_wait(&lock1);
			printf("CountA: %lu\n", countA);
			sem_post(&lock1);
			sem_wait(&lock2);
			printf("CountB: %lu\n", countB);
			sem_post(&lock2);
		}
		else if(strncmp(line,"unsafe",6)==0)
		{
			printf("CountA: %lu ( %lu | %lu )\n", countA, countA1, countA2);
			printf("CountB: %lu ( %lu | %lu )\n", countB, countB1, countB2);
		}
		else if(strncmp(line,"exit",4)==0)
		{
			free(line);
			break;
		}
		free(line);
		line=0;
	}
	running=0;
	thrd_join(thread1,0);
	thrd_join(thread2,0);
	sem_destroy(&rlock);
	sem_destroy(&lock1);
	sem_destroy(&lock2);
	printf("CountA: %lu ( %lu | %lu )\n", countA, countA1, countA2);
	printf("CountB: %lu ( %lu | %lu )\n", countB, countB1, countB2);
	return 0;
}