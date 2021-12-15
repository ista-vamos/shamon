#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "bank.h"

void selectAccount(User ** cur_user, Account** cur_acc, char ** message)
{
	char* line = NULL;
	size_t line_len = 0;
  	ssize_t lineSize = 0;
	Account_list * accounts = (*cur_user)->ops->getAccounts(*cur_user);
	printf("\e[1;1H\e[2J");
	printf("Select Account:\n");
	while(accounts!=NULL)
	{
		printf("%lu\n",accounts->element->number);
		accounts=accounts->next;
	}
	printf("L. Logout\n");
	printf("Select account no. or action: ");
	lineSize = getline(&line, &line_len, stdin);
	if(lineSize>0)
	{
		if(strcmp(line,"L\n")==0)
		{
			*cur_user=NULL;
			*message="Logged out!";
			free(line);
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
			free(line);
			return;
		}
	}
	else
	{
		if(line!=NULL)
		{
			free(line);
		}
		return;
	}
}

int main(int argc, char **args)
{
	Account* cur_acc = NULL;
	User * cur_user = NULL;
	int stop = 0;
  	ssize_t lineSize = 0;
	char *message=NULL;

	while(!stop)
	{
		printf("\e[1;1H\e[2J");
		if(cur_user==NULL)
		{
			if(message!=NULL)
			{
				printf("%s\n",message);
				message=NULL;
			}
			printf("Login: ");
			char* username= NULL;
			size_t username_len = 0;
  			lineSize = getline(&username, &username_len, stdin);
			if(lineSize>0)
			{
				username[lineSize-1]=0;
				char* pwd = NULL;
				size_t pwd_len = 0;
				printf("Password: ");
  				lineSize = getline(&pwd, &pwd_len, stdin);
				if(lineSize>0)
				{
					pwd[lineSize-1]=0;
					cur_user=Login(username,pwd);
					
					if(cur_user==NULL)
					{
						message="Invalid Login!";
					}
				}
				if(pwd!=NULL)
				{
					free(pwd);
				}
				continue;
			}
			else
			{
				if(username!=NULL)
				{
					free(username);
				}
				stop=1;
				break;
			}
		}
		else
		{
			printf("Welcome, %s!\n", cur_user->name);
			if(message!=NULL)
			{
				printf("%s\n",message);
				message=NULL;
			}
			if(cur_acc==NULL)
			{
				printf("\n");
				printf("1. Select Account\n");
				printf("0. Logout\n");
				printf("Select Action: ");
				char* line = NULL;
				size_t line_len = 0;
  				lineSize = getline(&line, &line_len, stdin);
				if(lineSize>0)
				{
					if(strcmp(line,"1\n")==0)
					{
						free(line);
						selectAccount(&cur_user, &cur_acc, &message);
						continue;
					}
					else if(strcmp(line,"0\n")==0)
					{
						cur_user=NULL;
						message="Logged out!";
						free(line);
						continue;
					}
					else
					{
						message="Invalid command!";
					}
				}
				else
				{
					if(line!=NULL)
					{
						free(line);
					}
					continue;
				}
			}
			else
			{
				printf("Selected account: %lu\n",cur_acc->number);
				printf("\n");
				printf("1. Deposit\n");
				printf("2. Withdraw\n");
				printf("3. Check Balance\n");
				printf("4. Transfer\n");
				printf("5. Select Different Account\n");
				printf("0. Logout\n");
				char* line = NULL;
				size_t line_len = 0;
  				lineSize = getline(&line, &line_len, stdin);
				if(lineSize>0)
				{
					if(strcmp(line, "1\n")==0)
					{
						printf("\e[1;1H\e[2J");
						printf("Deposit to Account %lu\n", cur_acc->number);
						printf("Amount: ");
						free(line);
						line=NULL;
						line_len=0;
  						lineSize = getline(&line, &line_len, stdin);
						if(lineSize>0)
						{
							long amount = atol(line);
							if(amount<1)
							{
								message="Deposit amount must be positive!";
								free(line);
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
						printf("\e[1;1H\e[2J");
						printf("Withdraw from Account %lu\n", cur_acc->number);
						printf("Amount: ");
						free(line);
						line=NULL;
						line_len=0;
  						lineSize = getline(&line, &line_len, stdin);
						if(lineSize>0)
						{
							long amount = atol(line);
							if(amount<1)
							{
								message="Withdrawal amount must be positive!";
								free(line);
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
						printf("\e[1;1H\e[2J");
						printf("Current balance on Account %lu:\n", cur_acc->number);
						printf("%lu\n", cur_acc->balance);
						printf("\n");
						printf("Press ENTER to return");
						free(line);
						line=NULL;
						line_len=0;
  						lineSize = getline(&line, &line_len, stdin);
					}
					else if(strcmp(line, "4\n")==0)
					{
						printf("\e[1;1H\e[2J");
						printf("Transfer from Account %lu:\n", cur_acc->number);
						printf("\n");
						printf("Receiving Account: ");
						free(line);
						line=NULL;
						line_len=0;
  						lineSize = getline(&line, &line_len, stdin);
						if(lineSize>0)
						{
							long otherAccNo = atol(line);
							Account* otherAcc = getAccount(otherAccNo);
							if(otherAcc!=NULL)
							{
								printf("\e[1;1H\e[2J");
								printf("Transfer from Account %lu\n", cur_acc->number);
								printf("           to Account %lu\n", otherAcc->number);
								printf("\n");
								printf("Amount: ");
								free(line);
								line=NULL;
								line_len=0;
								lineSize = getline(&line, &line_len, stdin);
								if(lineSize>0)
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
					if(line!=NULL)
					{
						free(line);
					}
					continue;
				}
				else
				{
					if(line!=NULL)
					{
						free(line);
					}
					continue;
				}
			}
		}
	}
}