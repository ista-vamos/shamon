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

int monitoring_thread(void* arg)
{
	monitor_buffer buffer=(monitor_buffer)arg;
	buffer_entry buffer_buffer[32];
	while(monitoring_active)
	{
		#ifdef SHM_DOMONITOR_NOWAIT
		size_t count = copy_events_nowait(buffer, buffer_buffer, 32);
		#else
		size_t count = copy_events_wait(buffer, buffer_buffer, 32);
		#endif
		for(size_t i=0;i<count;i++)
		{
			if(do_print)
			{
				if(buffer_buffer[i].kind==1)
				{
					printf("Write: %s\n", ((char*)(buffer_buffer[i].payload64_1))+sizeof(size_t)+sizeof(int64_t));
				}
				else
				{
					printf("Read: %s\n", ((char*)(buffer_buffer[i].payload64_1))+sizeof(size_t)+sizeof(int64_t));	
				}
			}
			free((void*)buffer_buffer[i].payload64_1);
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
