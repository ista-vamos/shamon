#ifndef CLIENTTYPE
#define CLIENTTYPE 1
#endif

#if CLIENTTYPE==1
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "bank.h"
#include "common.h"
#include <time.h>

void selectAccount(User ** cur_user, Account** cur_acc, char ** message)
{
	resetLinebuf();
	char* line = NULL;
	Account_list * accounts = (*cur_user)->ops->getAccounts(*cur_user);
	mprint("\e[1;1H\e[2J");
	mprint("Select Account:\n");
	while(accounts!=NULL)
	{
		mprint("%lu\n",accounts->element->number);
		accounts=accounts->next;
	}
	mprint("L. Logout\n");
	mprint("Select account no. or action: ");
	line = getLine();
	if(line!=NULL)
	{
		if(strcmp(line,"L\n")==0)
		{
			*cur_user=NULL;
			*message="Logged out!";
			return;
		}
		else
		{
			long accno = atol(line);
			accounts = (*cur_user)->ops->getAccounts(*cur_user);
			while(accounts!=NULL)
			{
				if(accounts->element->number==accno)
				{
					*cur_acc=accounts->element;
					break;
				}
				accounts=accounts->next;
			}
			if(*cur_acc==NULL)
			{
				*message="Invalid account number!";
			}
			return;
		}
	}
	else
	{
		return;
	}
}

int main(int argc, char **args)
{
	struct timespec begin, end;
	Account* cur_acc = NULL;
	User * cur_user = NULL;
	int stop = 0;
	char *message=NULL;

	#ifdef SHM_DOMONITOR
	initialize_application_buffer();
	intialize_thread_buffer(1,2);
	app_buffer_wait_for_client();
	#endif

	if (clock_gettime(CLOCK_MONOTONIC, &begin) == -1)
	{
		perror("clock_gettime");
	}
	while(!stop)
	{
		resetLinebuf();
		mprint("\e[1;1H\e[2J");
		if(cur_user==NULL)
		{
			if(message!=NULL)
			{
				mprint("%s\n",message);
				message=NULL;
			}
			mprint("Login: ");
			char* username= getLine();
			if(username!=NULL)
			{
				removeNewLineAtEnd(username);
				mprint("Password: ");
				char* pwd = getLine();
				if(pwd!=NULL)
				{
					removeNewLineAtEnd(pwd);
					cur_user=Login(username,pwd);
					
					if(cur_user==NULL)
					{
						message="Invalid Login!";
					}
				}
				continue;
			}
			else
			{
				stop=1;
				break;
			}
		}
		else
		{
			mprint("Welcome, %s!\n", cur_user->name);
			if(message!=NULL)
			{
				mprint("%s\n",message);
				message=NULL;
			}
			if(cur_acc==NULL)
			{
				mprint("\n");
				mprint("1. Select Account\n");
				mprint("0. Logout\n");
				mprint("Select Action: ");
				char* line = getLine();
				if(line!=NULL)
				{
					if(strcmp(line,"1\n")==0)
					{
						selectAccount(&cur_user, &cur_acc, &message);
						continue;
					}
					else if(strcmp(line,"0\n")==0)
					{
						cur_user=NULL;
						message="Logged out!";
						continue;
					}
					else
					{
						message="Invalid command!";
					}
				}
				else
				{
					continue;
				}
			}
			else
			{
				mprint("Selected account: %lu\n",cur_acc->number);
				mprint("\n");
				mprint("1. Deposit\n");
				mprint("2. Withdraw\n");
				mprint("3. Check Balance\n");
				mprint("4. Transfer\n");
				mprint("5. Select Different Account\n");
				mprint("0. Logout\n");
				char* line = getLine();
				if(line!=NULL)
				{
					if(strcmp(line, "1\n")==0)
					{
						mprint("\e[1;1H\e[2J");
						mprint("Deposit to Account %lu\n", cur_acc->number);
						mprint("Amount: ");
						line=getLine();
						if(line!=NULL)
						{
							long amount = atol(line);
							if(amount<1)
							{
								message="Deposit amount must be positive!";
								continue;
							}
							cur_acc->ops->deposit(cur_acc, amount);
							message="Deposit successful!";
						}
						else
						{
							message="Deposit aborted";
						}
					}
					else if(strcmp(line, "2\n")==0)
					{
						mprint("\e[1;1H\e[2J");
						mprint("Withdraw from Account %lu\n", cur_acc->number);
						mprint("Amount: ");
						line=getLine();
						if(line!=NULL)
						{
							long amount = atol(line);
							if(amount<1)
							{
								message="Withdrawal amount must be positive!";
								continue;
							}
							if(cur_acc->ops->withdraw(cur_acc, amount))
							{
								message="Withdrawal successful!";
							}
							else
							{
								message="Withdrawal failed!";
							}
						}
						else
						{
							message="Deposit aborted";
						}
					}
					else if(strcmp(line, "3\n")==0)
					{
						mprint("\e[1;1H\e[2J");
						mprint("Current balance on Account %lu:\n", cur_acc->number);
						mprint("%lu\n", cur_acc->balance);
						mprint("\n");
						mprint("Press ENTER to return");
						line=getLine();
					}
					else if(strcmp(line, "4\n")==0)
					{
						mprint("\e[1;1H\e[2J");
						mprint("Transfer from Account %lu:\n", cur_acc->number);
						mprint("\n");
						mprint("Receiving Account: ");
						line=getLine();
						if(line!=NULL)
						{
							long otherAccNo = atol(line);
							Account* otherAcc = getAccount(otherAccNo);
							if(otherAcc!=NULL)
							{
								mprint("\e[1;1H\e[2J");
								mprint("Transfer from Account %lu\n", cur_acc->number);
								mprint("           to Account %lu\n", otherAcc->number);
								mprint("\n");
								mprint("Amount: ");
								line=getLine();
								if(line!=NULL)
								{
									long amount = atol(line);
									if(amount>0)
									{
										if(cur_acc->ops->transfer(cur_acc, otherAcc, amount))
										{
											message="Transfer successful!";
										}
										else
										{
											message="Transfer failed!";
										}
									}
									else
									{
										message="Transfer amount must be positive!";
									}
								}
								else
								{
									message="Transfer aborted";
								}
							}
							else
							{
								message="Unknown account!";
							}
						}
						else
						{
							message="Transfer aborted";
						}
					}
					else if(strcmp(line, "5\n")==0)
					{
						selectAccount(&cur_user,&cur_acc,&message);
					}
					else if(strcmp(line, "0\n")==0)
					{
						cur_acc=NULL;
						cur_user=NULL;
						message = "Logged out!";
					}
					else
					{
						message="Invalid command!";
					}
					continue;
				}
				else
				{
					continue;
				}
			}
		}
	}
	if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
	{
		perror("clock_gettime");
	}
	long seconds = end.tv_sec - begin.tv_sec;
	long nanoseconds = end.tv_nsec - begin.tv_nsec;
	double elapsed = seconds + nanoseconds*1e-9;
	printf("\nTime: %lf seconds.\n", elapsed);

	#ifdef SHM_DOMONITOR
	close_thread_buffer();
	close_app_buffer();
	#endif
}
#endif