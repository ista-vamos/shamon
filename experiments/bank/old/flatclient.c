#include "bank.h"
#include "common.h"
#include <string.h>

#if CLIENTTYPE == 2

int processLogin(char **line, User **user, Account **account) {
    char *val = *line;
    val += strlen(val) + 1;
    char *username = val;
    val += strlen(val) + 1;
    char *pwd = val;
    val += strlen(val) + 1;
    char *acctno = val;
    val += strlen(val) + 1;
    *line = val;
    *user = Login(username, pwd);
    if (*user == NULL) {
        printf("Invalid login!\n");
        return 0;
    }
    *account = (*user)->ops->getAccount(*user, atol(acctno));
    if (*account == NULL) {
        printf("No matching account!\n");
        return 0;
    }
    return 1;
}
void deposit(char *line, int parts) {
    if (parts != 5) {
        printf("Usage: deposit [user] [pwd] [account] [amount]\n");
        return;
    }
    User    *user    = NULL;
    Account *account = NULL;
    if (processLogin(&line, &user, &account)) {
        uint64_t amount = atol(line);
        if (amount > 0) {
            account->ops->deposit(account, amount);
            printf("Deposit successful!\n");
        } else {
            printf("Deposit amount must be positive!\n");
        }
    }
}
void withdraw(char *line, int parts) {
    if (parts != 5) {
        printf("Usage: withdraw [user] [pwd] [account] [amount]\n");
        return;
    }
    User    *user    = NULL;
    Account *account = NULL;
    if (processLogin(&line, &user, &account)) {
        long amount = atol(line);
        if (amount > 0) {
            if (account->ops->withdraw(account, amount)) {
                printf("Withdrawal successful!\n");
            } else {
                printf("Withdrawal failed!\n");
            }
        } else {
            printf("Withdrawal amount must be positive!\n");
        }
    }
}
void checkBalance(char *line, int parts) {
    if (parts != 4) {
        printf("Usage: balance [user] [pwd] [account]\n");
        return;
    }
    User    *user    = NULL;
    Account *account = NULL;
    if (processLogin(&line, &user, &account)) {
        printf("Balance on account %lu: %lu\n", account->number,
               account->ops->checkBalance(account));
    }
}
void transfer(char *line, int parts) {
    if (parts != 6) {
        printf("Usage: transfer [user] [pwd] [account] [receiver-account] "
               "[amount]\n");
        return;
    }
    User    *user    = NULL;
    Account *account = NULL;
    if (processLogin(&line, &user, &account)) {
        Account *receiver = getAccount(atol(line));
        if (receiver != NULL) {
            line += strlen(line) + 1;
            long amount = atol(line);
            if (amount > 0) {
                if (account->number,
                    account->ops->transfer(account, receiver, amount)) {
                    printf("Transfer successful!\n");
                } else {
                    printf("Transfer failed!\n");
                }
            } else {
                printf("Transfer amount must be positive!\n");
            }
        } else {
            printf("Receiver account not found!\n");
        }
    }
}

int main(int argc, char **args) {
    int stop = 0;
    while (!stop) {
        resetLinebuf();
        char *line = getLine();
        if (line != NULL) {
            removeNewLineAtEnd(line);
            int parts = strsplit(line, ' ', 5);
            if (strcmp(line, "deposit") == 0) {
                deposit(line, parts);
            } else if (strcmp(line, "withdraw") == 0) {
                withdraw(line, parts);
            } else if (strcmp(line, "balance") == 0) {
                checkBalance(line, parts);
            } else if (strcmp(line, "transfer") == 0) {
                transfer(line, parts);
            } else if (strcmp(line, "help") == 0) {
                printf("Commands:\n\n");
                printf("deposit [user] [pwd] [account] [amount]\n");
                printf("withdraw [user] [pwd] [account] [amount]\n");
                printf("balance [user] [pwd] [account]\n");
                printf("transfer [user] [pwd] [account] [receiver-account] "
                       "[amount]\n");
                printf("help\n");
                printf("quit\n\n");
            } else if (strcmp(line, "quit") == 0) {
                stop = 1;
            }
        } else {
            stop = 1;
        }
    }
}

#endif