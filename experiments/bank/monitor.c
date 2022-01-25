#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <threads.h>
#include <string.h>
#include "../../fastbuf/shm_monitor.h"

int monitoring_active=1;
int do_print=0;
size_t processed_bytes=0;

typedef struct msgbuf {
	struct msgbuf *next;
	struct msgbuf *prev;
	char* textbuf;
} msgbuf;

void insert_message(msgbuf *buf, char* text)
{
	if(strlen(text)==0)
	{
		return;
	}
	if(buf->textbuf==NULL)
	{
		buf->textbuf=text;
	}
	else
	{
		msgbuf * newbuf = (msgbuf*)malloc(sizeof(msgbuf));
		newbuf->textbuf=text;
		newbuf->next=buf;
		newbuf->prev=buf->prev;
		newbuf->next->prev=newbuf;
		newbuf->prev->next=newbuf;
	}
}
char * buf_clear_to_string(msgbuf *buf)
{
	msgbuf *current=buf;
	size_t size=0;
	if(current->textbuf==NULL)
	{
		return "";
	}
	do
	{
		size+=strlen(current->textbuf);
		current=current->next;
	}while(current!=buf);
	char* ret = (char*)malloc(sizeof(char) * size+1);
	char* retpos=ret;
	do
	{
		size_t len = strlen(current->textbuf);
		memcpy(retpos, current->textbuf, len);
		retpos+=len;
		free(current->textbuf-sizeof(size_t)-sizeof(int64_t));
		msgbuf *next=current->next;
		if(current!=buf)
		{
			free(current);
		}
		else
		{
			current->textbuf=NULL;
			current->next=current;
			current->prev=current;
		}
		current=next;
	}while(current!=buf);
	*retpos=0;
	return ret;
}

void process_prompt_answer(char * prompt, char* answer)
{
	char* origprompt=prompt;
	if(strncmp("\e[1;1H\e[2J",prompt,10)==0)
	{
		prompt+=10;
	}
	if(do_print)
	{
		printf("PROMPT:\n%s\n--------------------\nANSWER:\n%s\n",prompt,answer);
	}
	free(origprompt);
	free(answer);
}


int monitoring_thread(void* arg)
{
	monitor_buffer buffer=(monitor_buffer)arg;
	buffer_entry buffer_buffer[32];
	msgbuf read_msg;
	msgbuf write_msg;
	read_msg.next=&read_msg;
	write_msg.next=&write_msg;
	read_msg.prev=&read_msg;
	write_msg.prev=&write_msg;
	read_msg.textbuf=NULL;
	write_msg.textbuf=NULL;
	while(monitoring_active)
	{
		#ifdef SHM_DOMONITOR_NOWAIT
		size_t count = copy_events_nowait(buffer, buffer_buffer, 32);
		#else
		size_t count = copy_events_wait(buffer, buffer_buffer, 32);
		#endif
		for(size_t i=0;i<count;i++)
		{
			if(buffer_buffer[i].kind==1)
			{
				if(read_msg.textbuf!=NULL)
				{
					char* prompt = buf_clear_to_string(&write_msg);
					char* answer = buf_clear_to_string(&read_msg);
					process_prompt_answer(prompt, answer);
				}
				insert_message(&write_msg, ((char*)(buffer_buffer[i].payload64_1))+sizeof(size_t)+sizeof(int64_t));
			}
			else
			{
				insert_message(&read_msg, ((char*)(buffer_buffer[i].payload64_1))+sizeof(size_t)+sizeof(int64_t));
			}
			// if(do_print)
			// {
			// 	if(buffer_buffer[i].kind==1)
			// 	{
			// 		printf("Write: %s\n", ((char*)(buffer_buffer[i].payload64_1))+sizeof(size_t)+sizeof(int64_t));
			// 	}
			// 	else
			// 	{
			// 		printf("Read: %s\n", ((char*)(buffer_buffer[i].payload64_1))+sizeof(size_t)+sizeof(int64_t));	
			// 	}
			// }
			//free((void*)buffer_buffer[i].payload64_1);
			processed_bytes+=buffer_buffer[i].payload64_2;
		}
	}
	return 0;
}

int register_monitored_thread(monitor_buffer buffer)
{
	thrd_t thrd;
	thrd_create(&thrd, &monitoring_thread, buffer);
}

int main(int argc, char** argv)
{
	if(argc<2)
	{
		printf("Usage: monitor [PID] {print}");
		return 1;
	}
	pid_t process_id = atoi(argv[1]);
	if(argc>2)
	{
		if(strncmp(argv[2],"print",6)==0)
		{
			do_print=1;
		}
	}
	
	monitored_process proc = attach_to_process(process_id, &register_monitored_thread);

	wait_for_process(proc);
	monitoring_active=0;
	printf("Processed bytes: %lu\n", processed_bytes);
	return 0;
}
