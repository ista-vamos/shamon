#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <threads.h>
#include <string.h>
#include "../../fastbuf/shm_monitor.h"

int monitoring_active = 1;
int do_print = 0;
size_t processed_bytes = 0;

typedef struct msgbuf
{
	struct msgbuf *next;
	struct msgbuf *prev;
	char *textbuf;
	size_t offset;
} msgbuf;

int64_t acct_changes[4] = {0, 0, 0, 0};
int64_t acct_balances[4] = {-1, -1, -1, -1};
int64_t acct_limits[4] = {-1, -1, -1, -1};

void insert_message(msgbuf *buf, char *text)
{
	if (buf->textbuf == NULL)
	{
		buf->textbuf = text;
		buf->offset = sizeof(size_t)+sizeof(int64_t);
	}
	else
	{
		msgbuf *newbuf = (msgbuf *)malloc(sizeof(msgbuf));
		newbuf->textbuf = text;
		newbuf->offset=sizeof(size_t) + sizeof(int64_t);
		newbuf->next = buf;
		newbuf->prev = buf->prev;
		newbuf->next->prev = newbuf;
		newbuf->prev->next = newbuf;
	}
}

char *buf_get_upto(msgbuf *buf, char* delim)
{
	msgbuf *current = buf;
	size_t size = 0;
	if (current->textbuf == NULL)
	{
		return NULL;
	}
	do
	{
		char *nlpos = strstr(current->textbuf, delim);
		if (nlpos != NULL)
		{
			intptr_t len = (((intptr_t)nlpos) - ((intptr_t)current->textbuf)) + strlen(delim);
			char *ret = (char *)malloc(size + len + 1);
			memcpy(ret + size, current->textbuf, len);
			ret[size + len] = 0;
			size_t curlen = strlen(current->textbuf);
			if (curlen == len)
			{
				current->next->prev = current->prev;
				current->prev->next = current->next;
				free(current->textbuf - current->offset);
				current->textbuf = NULL;
				if (current == buf)
				{
					if (current->next != current)
					{
						current->textbuf = current->next->textbuf;
						current->offset = current->next->offset;
						msgbuf *newnext = current->next->next;
						newnext->prev = current;
						free(current->next);
						current->next = newnext;
					}
					return ret;
				}
				msgbuf *prev = current->prev;
				free(current);
				current = prev;
			}
			else
			{
				current->textbuf+=len;
				current->offset+=len;
				if (current == buf)
				{
					return ret;
				}
				current = current->prev;
			}
			while (1)
			{
				size_t curlen = strlen(current->textbuf);
				size -= curlen;
				memcpy(ret + size, current->textbuf, curlen);
				free(current->textbuf - current->offset);
				current->textbuf = NULL;
				current->next->prev = current->prev;
				current->prev->next = current->next;
				if (current == buf)
				{
					if (current->next != current)
					{
						current->textbuf = current->next->textbuf;
						current->offset = current->next->offset;
						msgbuf *newnext = current->next->next;
						newnext->prev = current;
						free(current->next);
						current->next = newnext;
					}
					return ret;
				}
				msgbuf *prev = current->prev;
				free(current);
				current = prev;
			}
		}
		size += strlen(current->textbuf);
		current = current->next;
	} while (current != buf);
	return NULL;
}

char *buf_get_line(msgbuf *buf)
{
	return buf_get_upto(buf, "\n");
}
char *buf_get_prompt(msgbuf *buf)
{
	return buf_get_upto(buf, ">");
}

int64_t pending_withdrawal = -1;
int pending_withdrawal_source = -1;
int64_t pending_transfer = -1;
int pending_transfer_source = -1;
int pending_transfer_target = -1;

void process_prompt_answer(char *prompt, char *answer)
{
	char *origprompt = prompt;
	if (strncmp("\e[1;1H\e[2J", prompt, 10) == 0)
	{
		prompt += 10;
	}
	if (strncmp("Deposit", prompt, 7) == 0)
	{
		if (strncmp(prompt + 19, "10000", 5) == 0)
		{
			int acctidx = atoi(prompt + 24) - 1;
			if (acctidx >= 0 && acctidx < 4)
			{
				int64_t amount = atol(answer);
				if (amount >= 0)
				{
					if (acct_balances[acctidx] < 0)
					{
						acct_changes[acctidx] += amount;
						if (acct_limits[acctidx] >= 0)
						{
							acct_limits[acctidx] += amount;
						}
					}
					else
					{
						acct_balances[acctidx] += amount;
					}
				}
			}
		}
	}
	else if (strncmp("Withdraw", prompt, 8) == 0)
	{
		if (strncmp(prompt + 22, "10000", 5) == 0)
		{
			int acctidx = atoi(prompt + 27) - 1;
			if (acctidx >= 0 && acctidx < 4)
			{
				int64_t amount = atol(answer);
				if (amount >= 0)
				{
					pending_withdrawal = amount;
					pending_withdrawal_source = acctidx;
				}
			}
		}
	}
	else if (strncmp("Transfer", prompt, 8) == 0)
	{
		char *nextline = strstr(prompt, "\n");
		if (nextline != NULL)
		{
			nextline += 1;
			if (strncmp("           to Account", nextline, 21) == 0)
			{
				if (strncmp(prompt + 22, "10000", 5) == 0 && strncmp(nextline + 22, "10000", 5) == 0)
				{
					int acctidx1 = atoi(prompt + 27) - 1;
					int acctidx2 = atoi(nextline + 27) - 1;
					int64_t amount = atol(answer);
					if (amount > 0)
					{
						if (acctidx1 >= 0 && acctidx1 < 4)
						{
							pending_transfer = amount;
							pending_transfer_source = acctidx1;
						}
						if (acctidx2 >= 0 && acctidx2 < 4)
						{
							pending_transfer = amount;
							pending_transfer_target = acctidx2;
						}
					}
				}
			}
		}
	}
	else if (strncmp("Current balance", prompt, 15) == 0)
	{
		char *nextline = strstr(prompt, "\n");
		if (nextline != NULL)
		{
			nextline += 1;
			int64_t amount = atol(nextline);
			if (strncmp(prompt + 27, "10000", 5) == 0)
			{
				int acctidx = atoi(prompt + 32) - 1;
				if (acctidx >= 0 && acctidx < 4)
				{
					if (acct_balances[acctidx] >= 0 && acct_balances[acctidx] != amount)
					{
						printf("WARNING: found mismatched account balance for account 10000%i. Expected %li, but found %li\n", acctidx, acct_balances[acctidx], amount);
					}
					else if (acct_balances[acctidx] >= 0)
					{
						if (do_print)
						{
							printf("Checked account balance for account 10000%i; result %li matches expectation", acctidx, amount);
						}
					}
					else
					{
						if (do_print)
						{
							printf("Checked account balance for account 10000%i; result %li", acctidx, amount);
						}
					}
					acct_balances[acctidx] = amount;
					acct_changes[acctidx] = 0;
					acct_limits[acctidx] = -1;
				}
			}
		}
	}
	else if (do_print)
	{
		printf("PROMPT:\n%s\n--------------------\nANSWER:\n%s\n", prompt, answer);
	}
	free(origprompt);
	free(answer);
}

void check_state(msgbuf *write_msg, msgbuf *read_msg)
{
	while(1)
	{
		if (pending_withdrawal >= 0)
		{
			char *line = buf_get_line(write_msg);
			while (line != NULL)
			{
				size_t lineoff = 0;
				if (strncmp("\e[1;1H\e[2J", line, 10) == 0)
				{
					line += 10;
					lineoff = 10;
				}
				if (strlen(line) <= 1 || strncmp(line, "Welcome, ", 9) == 0)
				{
					free(line - lineoff);
					line = buf_get_line(write_msg);
					continue;
				}
				else if (strncmp(line, "Withdrawal successful!", 22) == 0)
				{
					if (acct_balances[pending_withdrawal_source] >= 0)
					{
						if (acct_balances[pending_withdrawal_source] >= pending_withdrawal)
						{
							acct_balances[pending_withdrawal_source] -= pending_withdrawal;
						}
						else
						{
							printf("WARNING: Withdrawal of %li from account 10000%i allowed despite expected balance of %li!\n", pending_withdrawal, pending_withdrawal_source, acct_balances[pending_withdrawal_source]);
							acct_balances[pending_withdrawal_source] = -1;
						}
					}
					else if (acct_limits[pending_withdrawal_source] >= 0)
					{
						if (acct_limits[pending_withdrawal_source] >= pending_withdrawal)
						{
							acct_limits[pending_withdrawal_source] -= pending_withdrawal;
						}
						else
						{
							printf("WARNING: Withdrawal of %li from account 10000%i allowed despite previously determined upper balance limit of %li!\n", pending_withdrawal, pending_withdrawal_source, acct_limits[pending_withdrawal_source]);
							acct_limits[pending_withdrawal_source] = -1;
						}
					}
					if (acct_changes[pending_withdrawal_source] > pending_withdrawal)
					{
						acct_changes[pending_withdrawal_source] -= pending_withdrawal;
					}
					else
					{
						acct_changes[pending_withdrawal_source] = 0;
					}
					pending_withdrawal = -1;
					pending_withdrawal_source = -1;
					free(line - lineoff);
					break;
				}
				else if (strncmp(line, "Withdrawal failed!", 18) == 0)
				{
					if (acct_balances[pending_withdrawal_source] >= 0)
					{
						if (acct_balances[pending_withdrawal_source] >= pending_withdrawal)
						{
							printf("WARNING: Withdrawal of %li from account 10000%i failed despite expected balance of %li!\n", pending_withdrawal, pending_withdrawal_source, acct_balances[pending_withdrawal_source]);
						}
					}
					else
					{
						if (acct_limits[pending_withdrawal_source] >= pending_withdrawal || acct_limits[pending_withdrawal_source] < 0)
						{
							acct_limits[pending_withdrawal_source] = pending_withdrawal - 1;
						}
						if (acct_changes[pending_withdrawal_source] >= pending_withdrawal)
						{
							printf("WARNING: Withdrawal of %li from account 10000%i failed despite previously determined lower balance bound of of %li!\n", pending_withdrawal, pending_withdrawal_source, acct_changes[pending_withdrawal_source]);
						}
					}
					pending_withdrawal = -1;
					pending_withdrawal_source = -1;
					free(line - lineoff);
					break;
				}
				else
				{
					pending_withdrawal = -1;
					pending_withdrawal_source = -1;
					free(line - lineoff);
					break;
				}
			}
		}
		if (pending_transfer >= 0)
		{
			char *line = buf_get_line(write_msg);
			while (line != NULL)
			{
				size_t lineoff = 0;
				if (strncmp("\e[1;1H\e[2J", line, 10) == 0)
				{
					line += 10;
					lineoff = 10;
				}
				if (strlen(line) <= 1 || strncmp(line, "Welcome, ", 9) == 0)
				{
					free(line - lineoff);
					line = buf_get_line(write_msg);
					continue;
				}
				else if (strncmp(line, "Transfer successful!", 20) == 0)
				{
					if (pending_transfer_source >= 0)
					{
						if (acct_balances[pending_transfer_source] >= 0)
						{
							if (acct_balances[pending_transfer_source] >= pending_transfer)
							{
								acct_balances[pending_transfer_source] -= pending_transfer;
							}
							else
							{
								printf("WARNING: Transfer of %li from account 1000%i to account 1000%i allowed despite insufficient expected balance %li!\n", pending_transfer, pending_transfer_source, pending_transfer_target, acct_balances[pending_transfer_source]);
								acct_balances[pending_transfer_source] = -1;
								acct_changes[pending_transfer_source] = 0;
								acct_limits[pending_transfer_source] = -1;
							}
						}
						else
						{
							acct_changes[pending_transfer_source] += pending_transfer;
							if (acct_limits[pending_transfer_source] >= 0)
							{
								if (acct_limits[pending_transfer_source] >= pending_transfer)
								{
									acct_limits[pending_transfer_source] -= pending_transfer;
								}
								else
								{
									printf("WARNING: Transfer of %li from account 1000%i to account 1000%i allowed despite previously determined upper balance limit %li!\n", pending_transfer, pending_transfer_source, pending_transfer_target, acct_limits[pending_transfer_source]);
									acct_limits[pending_transfer_source] = -1;
								}
							}
							if (acct_changes[pending_transfer_source] >= pending_transfer)
							{
								acct_changes[pending_transfer_source] -= pending_transfer;
							}
							else
							{
								acct_changes[pending_transfer_source] = 0;
							}
						}
					}
					if (pending_transfer_target >= 0)
					{
						if (acct_balances[pending_transfer_target] >= 0)
						{
							acct_balances[pending_transfer_target] += pending_transfer;
						}
						else if (acct_limits[pending_transfer_target] >= 0)
						{
							acct_limits[pending_transfer_target] += pending_transfer;
						}
						else
						{
							acct_changes[pending_transfer_target] += pending_transfer;
						}
					}
					pending_transfer = -1;
					pending_transfer_source = -1;
					pending_transfer_target = -1;
					free(line - lineoff);
					break;
				}
				else if (strncmp(line, "Transfer failed!", 16) == 0)
				{
					if (pending_transfer_source >= 0)
					{
						if (acct_balances[pending_transfer_source] >= pending_transfer)
						{
							printf("WARNING: Transfer of %li from account 1000%i to account 1000%i failed despite previously determined balance %li!\n", pending_transfer, pending_transfer_source, pending_transfer_target, acct_balances[pending_transfer_source]);
							acct_balances[pending_transfer_source] = -1;
						}
						if (acct_changes[pending_transfer_source] >= pending_transfer)
						{
							printf("WARNING: Transfer of %li from account 1000%i to account 1000%i failed despite previously determined lower balance bound %li!\n", pending_transfer, pending_transfer_source, pending_transfer_target, acct_changes[pending_transfer_source]);
							acct_changes[pending_transfer_source] = 0;
						}
						if (acct_limits[pending_transfer_source] >= pending_transfer || acct_limits[pending_transfer_source] < 0)
						{
							acct_limits[pending_transfer_target] = pending_transfer - 1;
						}
					}
					pending_transfer = -1;
					pending_transfer_source = -1;
					pending_transfer_target = -1;
					free(line - lineoff);
					break;
				}
				else
				{
					pending_transfer = -1;
					pending_transfer_source = -1;
					pending_transfer_target = -1;
					free(line - lineoff);
					break;
				}
			}
		}
		static char * prompt=NULL;
		static char * answer = NULL;
		if(prompt==NULL)
		{
			prompt = buf_get_upto(write_msg, ">");
		}
		if(answer==NULL)
		{
			answer= buf_get_line(read_msg);
		}
		if(prompt!=NULL&&answer!=NULL)
		{
			process_prompt_answer(prompt, answer);
			prompt=NULL;
			answer=NULL;
		}
		else
		{
			break;
		}
	}
}

int monitoring_thread(void *arg)
{
	monitor_buffer buffer = (monitor_buffer)arg;
	buffer_entry buffer_buffer[32];
	msgbuf read_msg;
	msgbuf write_msg;
	read_msg.next = &read_msg;
	write_msg.next = &write_msg;
	read_msg.prev = &read_msg;
	write_msg.prev = &write_msg;
	read_msg.textbuf = NULL;
	write_msg.textbuf = NULL;
	while (monitoring_active)
	{
#ifdef SHM_DOMONITOR_NOWAIT
		size_t count = copy_events_nowait(buffer, buffer_buffer, 32);
#else
		size_t count = copy_events_wait(buffer, buffer_buffer, 32);
#endif
		for (size_t i = 0; i < count; i++)
		{
			if (buffer_buffer[i].kind == 1)
			{
				insert_message(&write_msg, ((char *)(buffer_buffer[i].payload64_1)) + sizeof(size_t) + sizeof(int64_t));
			}
			else
			{
				insert_message(&read_msg, ((char *)(buffer_buffer[i].payload64_1)) + sizeof(size_t) + sizeof(int64_t));
			}
			check_state(&write_msg, &read_msg);
			processed_bytes += buffer_buffer[i].payload64_2;
		}
	}
	return 0;
}

int register_monitored_thread(monitor_buffer buffer)
{
	thrd_t thrd;
	thrd_create(&thrd, &monitoring_thread, buffer);
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Usage: monitor [PID] {print}");
		return 1;
	}
	pid_t process_id = atoi(argv[1]);
	if (argc > 2)
	{
		if (strncmp(argv[2], "print", 6) == 0)
		{
			do_print = 1;
		}
	}

	monitored_process proc = attach_to_process(process_id, &register_monitored_thread);

	wait_for_process(proc);
	monitoring_active = 0;
	printf("Processed bytes: %lu\n", processed_bytes);
	return 0;
}
