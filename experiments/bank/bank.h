#pragma once
#include <stdint.h>

#define TXTYPE_DEPOSIT  1
#define TXTYPE_WITHDRAW 2
#define TXTYPE_TRANSFER 3

struct Account;

typedef struct Account_ops {
    void (*deposit)(struct Account *self, int64_t amount);
    int (*withdraw)(struct Account *self, int64_t amount);
    int64_t (*checkBalance)(struct Account *self);
    int (*transfer)(struct Account *self, struct Account *other,
                    int64_t amount);
} Account_ops;

typedef struct Account {
    Account_ops *ops;
    uint64_t     number;
    int64_t      balance;
    uint64_t     lastversion;
} Account;

typedef struct Account_list {
    struct Account_list *next;
    Account             *element;
} Account_list;

struct User;

typedef struct User_ops {
    Account_list *(*getAccounts)(struct User *self);
    Account *(*getAccount)(struct User *self, uint64_t number);
} User_ops;

typedef struct User {
    User_ops *ops;
    uint64_t  id;
    char     *name;
    char     *pwd;
} User;

typedef struct User_list {
    struct User_list *next;
    User             *element;
} User_list;

User    *Login(char *name, char *pwd);
Account *getAccount(uint64_t number);