#include "bank.h"
#include "common.h"

Account_list *stdGetAccounts(User *self);
Account      *stdGetAccount(User *self, uint64_t number);

void stdDeposit(Account *self, int64_t amount) {
    self->balance += amount;
    self->lastversion++;
}
int stdWithdraw(Account *self, int64_t amount) {
    if (self->balance >= amount) {
        self->balance -= amount;
        self->lastversion++;
        return 1;
    }
    return 0;
}
int64_t stdCheckBalance(Account *self) {
    return self->balance;
}
int stdTransfer(Account *self, Account *other, int64_t amount) {
    if (self->balance >= amount) {
        self->balance -= amount;
        other->balance += amount;
        self->lastversion++;
        other->lastversion++;
        return 1;
    }
    return 0;
}

static User_ops stduserops = {.getAccounts = &stdGetAccounts,
                              .getAccount  = &stdGetAccount};

static Account_ops stdaccountops = {.deposit      = &stdDeposit,
                                    .withdraw     = &stdWithdraw,
                                    .checkBalance = &stdCheckBalance,
                                    .transfer     = &stdTransfer};

static User firstuser = {
    .ops = &stduserops, .id = 1, .name = "user1", .pwd = "pwd1"};
static User seconduser = {
    .ops = &stduserops, .id = 2, .name = "user2", .pwd = "pwd2"};

static User_list secondulentry = {.next = NULL, .element = &seconduser};
static User_list firstulentry = {.next = &secondulentry, .element = &firstuser};

static User_list *users = &firstulentry;

User *Login(char *name, char *pwd) {
    User_list *current = users;
    while (current != NULL) {
        int namematch = strcmp(current->element->name, name) == 0;
        int pwdmatch  = strcmp(pwd, current->element->pwd) == 0;
        if (namematch && pwdmatch) {
            return current->element;
        }
        current = current->next;
    }
    return NULL;
}

static Account account1 = {
    .ops = &stdaccountops, .number = 100001, .balance = 0, .lastversion = 1};
static Account account2 = {
    .ops = &stdaccountops, .number = 100002, .balance = 100, .lastversion = 1};
static Account account3 = {
    .ops = &stdaccountops, .number = 100003, .balance = 54, .lastversion = 1};
static Account account4 = {
    .ops = &stdaccountops, .number = 100004, .balance = 0, .lastversion = 1};

static Account_list  acclst1_3 = {.next = NULL, .element = &account3};
static Account_list  acclst1_2 = {.next = &acclst1_3, .element = &account2};
static Account_list  acclst1_1 = {.next = &acclst1_2, .element = &account1};
static Account_list *acclst1   = &acclst1_1;

static Account_list  acclst2_2 = {.next = NULL, .element = &account2};
static Account_list  acclst2_1 = {.next = &acclst1_3, .element = &account4};
static Account_list *acclst2   = &acclst2_1;

static Account_list acclst_4 = {.next = NULL, .element = &account4};
static Account_list acclst_3 = {.next = &acclst_4, .element = &account3};
static Account_list acclst_2 = {.next = &acclst_3, .element = &account2};
static Account_list acclst_1 = {.next = &acclst_2, .element = &account1};

static Account_list *all_accounts = &acclst_1;

Account_list *stdGetAccounts(User *self) {
    if (self == &firstuser) {
        return acclst1;
    } else if (self == &seconduser) {
        return acclst2;
    }
    return NULL;
}
Account *stdGetAccount(User *self, uint64_t number) {
    Account_list *current = self->ops->getAccounts(self);
    while (current != NULL) {
        if (current->element->number == number) {
            return current->element;
        }
        current = current->next;
    }
    return NULL;
}

Account *getAccount(uint64_t number) {
    Account_list *current = all_accounts;
    while (current != NULL) {
        if (current->element->number == number) {
            return current->element;
        }
        current = current->next;
    }
    return NULL;
}