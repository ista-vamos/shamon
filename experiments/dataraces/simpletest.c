#include <threads.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

mtx_t lock1, lock2, rlock;
uint64_t countA, countB, countA1, countB1, countA2, countB2;
int running=1;
int Thread1(void* arg)
{
	int r;
	while(running)
	{
		mtx_lock(&rlock);
		r=rand()%2;
		mtx_unlock(&rlock);
		if(r%2)
		{
			mtx_lock(&lock1);
			countA++;
			mtx_unlock(&lock1);
			countA1++;
		}
		else
		{
			mtx_lock(&lock2);
			countB++;
			mtx_unlock(&lock2);
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
		mtx_lock(&rlock);
		r=rand()%2;
		mtx_unlock(&rlock);
		if(r%2)
		{
			mtx_lock(&lock1);
			countA++;
			mtx_unlock(&lock1);
			countA2++;
		}
		else
		{
			mtx_lock(&lock2);
			countB++;
			mtx_unlock(&lock2);
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
	mtx_init(&rlock, mtx_plain);
	mtx_init(&lock1, mtx_plain);
	mtx_init(&lock2, mtx_plain);
	thrd_create(&thread1,Thread1,0);
	thrd_create(&thread2,Thread2,0);
	while(getline(&line, &linelen, stdin)>=0)
	{
		if(strncmp(line,"safe",4)==0)
		{
			mtx_lock(&lock1);
			printf("CountA: %lu\n", countA);
			mtx_unlock(&lock1);
			mtx_lock(&lock2);
			printf("CountB: %lu\n", countB);
			mtx_unlock(&lock2);
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
	mtx_destroy(&rlock);
	mtx_destroy(&lock1);
	mtx_destroy(&lock2);
	printf("CountA: %lu ( %lu | %lu )\n", countA, countA1, countA2);
	printf("CountB: %lu ( %lu | %lu )\n", countB, countB1, countB2);
	return 0;
}