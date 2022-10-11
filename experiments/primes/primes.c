#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct ulonglist {
	unsigned long number;
	struct ulonglist *next;
} intlist;

int isPrime(long number)
{
	intlist base = { .number = 2, .next = NULL };
	intlist *last = &base;
	unsigned long current=3;
	if(number==2)
	{
		return 1;
	}
	while(current<=number)
	{
		intlist *curnode=&base;
		int found=0;
		while(curnode!=NULL)
		{
			if(current%curnode->number==0)
			{
				found=1;
				break;
			}
			curnode=curnode->next;
		}
		if(!found)
		{
			if(current==number)
			{
				return 1;
			}
			else
			{
				intlist *newnode = (intlist*)malloc(sizeof(intlist));
				newnode->number=current;
				newnode->next=NULL;
				last->next=newnode;
				last=newnode;
			}
		}
		current+=2;
	}
	last=base.next;
	while(last!=NULL)
	{
		intlist *node=last; 
		last=last->next;
		free(node);
	}
	return 0;
}

long findPrime(int target)
{
	intlist base = { .number = 2, .next = NULL };
	intlist *last = &base;
	unsigned long count=1;
	unsigned long current=3;
	if(target<=1)
	{
		return 2;
	}
	while(count<target)
	{
		intlist *curnode=&base;
		int found=0;
		while(curnode!=NULL)
		{
			if(current%curnode->number==0)
			{
				found=1;
				break;
			}
			curnode=curnode->next;
		}
		if(!found)
		{
			intlist *newnode = (intlist*)malloc(sizeof(intlist));
			newnode->number=current;
			newnode->next=NULL;
			last->next=newnode;
			last=newnode;
			count++;
		}
		current++;
	}
	current=last->number;
	last=base.next;
	while(last!=NULL)
	{
		intlist *node=last; 
		last=last->next;
		free(node);
	}
	return current;
}

int main(int argc, char **argv)
{
	struct timespec begin, end;
	int target=10;
	int repeat=0;
	int interactive=0;
	int argcnt=argc-1;
	int rcount=0;
	while(argcnt>0)
	{
		if(strcmp(argv[argc-argcnt],"-i")==0)
		{
			interactive=1;
		}
		else if(strcmp(argv[argc-argcnt],"-r")==0)
		{
			argcnt--;
			repeat=atoi(argv[argc-argcnt]);
		}
		else
		{
			target=atoi(argv[argc-argcnt]);
		}
		argcnt--;
	}
	if(repeat==0)
	{
		repeat=target;
	}
	if(interactive)
	{
		char buffer[256];
		char ipb[8];
		while(1)
		{
			int index;
			if(fgets(buffer, 256, stdin)==NULL)
			{
				printf("byeeof\n");
				break;
			}
			if(strcmp(buffer,"quit\n")==0)
			{
				printf("byequit\n");
				break;
			}
			strncpy(ipb, buffer, 8);
			ipb[7]=0;
			if(strcmp(ipb,"isprime")==0)
			{
				long num;
				if(strlen(buffer)<=8)
				{
					continue;
				}
				num=atol(buffer+8);
				printf("%li is%s prime\n", num, (isPrime(num)?"":" not"));
				continue;
			}
			index=atoi(buffer);
			if(index<1)
			{
				index=1;
			}
			printf("#%i: %lu\n", index, findPrime(index));
		}
	}
	else
	{
	        if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin) == -1) {
		        perror("clock_gettime");
                        return 1;
                }
		intlist base = { .number = 2, .next = NULL };
		intlist *last = &base;

		unsigned long current=3;
		unsigned long count=1;
		if(target>0)
		{
			printf("#1: 2\n");
		}
		while(count<target)
		{
			intlist *curnode=&base;
			int found=0;
			while(curnode!=NULL)
			{
				if(current%curnode->number==0)
				{
					found=1;
					break;
				}
				curnode=curnode->next;
			}
			if(!found)
			{
				intlist *newnode = (intlist*)malloc(sizeof(intlist));
				newnode->number=current;
				newnode->next=NULL;
				last->next=newnode;
				last=newnode;
				count++;
				printf("#%lu: %lu\n", count, current);
				rcount++;
				if(rcount==repeat)
				{
					rcount=0;
					curnode=base.next;
					while(curnode!=NULL)
					{
						last=curnode;
						curnode=curnode->next;
						free(last);
					}
					last=&base;
					last->next=NULL;
					current=2;
				}
			}
			current++;
		}

		if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1) {
			perror("clock_gettime");
				return 1;
		}
		long seconds = end.tv_sec - begin.tv_sec;
		long nanoseconds = end.tv_nsec - begin.tv_nsec;
		double elapsed = seconds + nanoseconds*1e-9;
		fprintf(stderr, "time: %lf seconds.\n", elapsed);
	}
}

