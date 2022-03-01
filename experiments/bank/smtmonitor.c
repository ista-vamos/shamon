#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <threads.h>
#include <string.h>
#include <stdatomic.h>
#include "../../fastbuf/shm_monitor.h"
#include "smt.h"
#define EVBUFSIZE 512
//#define EVENTDEBUG

int do_print=0;
int do_print_smt=0;
int do_print_unsat_smt=0;
_Atomic uint evpos;
_Atomic uint evcount;
mtx_t evwait_mutex;
cnd_t evwait_cond;


typedef struct msgbuf
{
	struct msgbuf *next;
	struct msgbuf *prev;
	char *textbuf;
	size_t offset;
} msgbuf;

uint64_t processed_bytes=0;
int monitoring_active=1;

typedef enum _event_kind { EV_NOTHING, EV_HOLE, EV_DEPOSIT, EV_WITHDRAW, EV_WITHDRAW_FAIL, EV_CHECKBALANCE, EV_TRANSFER, EV_TRANSFER_FAIL } event_kind;
typedef struct _event_head
{
	event_kind kind;
	#ifdef EVENTDEBUG
	uint64_t id;
	int index;
	#endif
} event_head;

typedef struct _hole_event
{
} hole_event;

typedef struct _deposit_event
{
	uint64_t account;
	uint64_t amount;
} deposit_event;

typedef struct _withdraw_event
{
	uint64_t account;
	uint64_t amount;
} withdraw_event;

typedef struct _checkbalance_event
{
	uint64_t account;
	uint64_t amount;
} checkbalance_event;

typedef struct _transfer_event
{
	uint64_t account;
	uint64_t target;
	uint64_t amount;
} transfer_event;

typedef struct _event
{
	event_head head;
	union
	{
		hole_event hole;
		deposit_event deposit;
		withdraw_event withdraw;
		checkbalance_event checkbalance;
		transfer_event transfer;
	} data;
} event;

#define ACTUALEVBUFSIZE (EVBUFSIZE*3)
event evbuf[ACTUALEVBUFSIZE];
event * const evbuf1=evbuf;
event * const evbuf2=evbuf+EVBUFSIZE;

int evwritepos=0;
int localevcount=0;
int writereset=-1;
#ifdef EVENTDEBUG
uint64_t eventid=0;
#endif
FILE* evparselog=NULL;


void printevent(FILE* file, event *ev)
{
	#ifdef EVENTDEBUG
	fprintf(file, "%lu/%i: ", ev->head.id, ev->head.index);
	#endif
	switch(ev->head.kind)
	{
		case EV_NOTHING:
		break;
		case EV_HOLE:
		fprintf(file, "HOLE()\n");
		break;
		case EV_CHECKBALANCE:
		fprintf(file, "CHECKBALANCE(%lu, %lu)\n", ev->data.checkbalance.account, ev->data.checkbalance.amount);
		break;
		case EV_DEPOSIT:
		fprintf(file, "DEPOSIT(%lu, %lu)\n", ev->data.deposit.account, ev->data.deposit.amount);
		break;
		case EV_WITHDRAW:
		fprintf(file, "WITHDRAW(%lu, %lu)\n", ev->data.withdraw.account, ev->data.withdraw.amount);
		break;
		case EV_WITHDRAW_FAIL:
		fprintf(file, "WITHDRAWFAIL(%lu, %lu)\n", ev->data.withdraw.account, ev->data.withdraw.amount);
		break;
		case EV_TRANSFER:
		fprintf(file, "TRANSFER(%lu, %lu, %lu)\n", ev->data.transfer.account,  ev->data.transfer.target, ev->data.transfer.amount);
		break;
		case EV_TRANSFER_FAIL:
		fprintf(file, "TRANSFERFAIL(%lu, %lu, %lu)\n", ev->data.transfer.account,  ev->data.transfer.target, ev->data.transfer.amount);
		break;
	}
}

event* get_next_event_place()
{
	if(evwritepos%EVBUFSIZE==0)
	{
		if(localevcount>=EVBUFSIZE*2)
		{
			localevcount=atomic_load_explicit(&evcount, memory_order_acquire);
			if(localevcount>=EVBUFSIZE*2)
			{
				localevcount=atomic_fetch_sub_explicit(&evcount, EVBUFSIZE, memory_order_acq_rel)-EVBUFSIZE;
				if(localevcount<EVBUFSIZE)
				{
					localevcount=atomic_fetch_add_explicit(&evcount, EVBUFSIZE, memory_order_acq_rel)+EVBUFSIZE;
					mtx_lock(&evwait_mutex);
					cnd_broadcast(&evwait_cond);
					mtx_unlock(&evwait_mutex);
				}
				else
				{
					if(evwritepos==0)
					{
						evwritepos=ACTUALEVBUFSIZE-EVBUFSIZE;
					}
					else
					{
						evwritepos-=EVBUFSIZE;
					}
					writereset=evwritepos;
					evbuf[evwritepos].head.kind=EV_HOLE;
					#ifdef EVENTDEBUG
					evbuf[evwritepos].head.id=eventid++;
					#endif
					if(evparselog!=NULL)
					{
						printevent(evparselog, &evbuf[evwritepos]);
					}
					evwritepos++;
				}
			}
		}
	}
	return evbuf+evwritepos;
}
void commit_event()
{
	if(evparselog!=NULL)
	{
		printevent(evparselog, &evbuf[evwritepos]);
	}
	int countadd=1;
	if(writereset>=0)
	{
		atomic_store_explicit(&evpos, writereset, memory_order_release);
		writereset=-1;
		countadd++;
	}
	localevcount=atomic_fetch_add_explicit(&evcount, countadd, memory_order_acq_rel)+countadd;
	if(localevcount<=countadd)
	{
		mtx_lock(&evwait_mutex);
		cnd_broadcast(&evwait_cond);
		mtx_unlock(&evwait_mutex);
	}
	evwritepos=(evwritepos+1)%ACTUALEVBUFSIZE;
}

void record_deposit_event(uint64_t account, uint64_t amount)
{
	if(do_print)
	{
		printf("DEPOSIT: %lu to %lu\n", amount, account);
	}
	event* nextev = get_next_event_place();
	nextev->head.kind=EV_DEPOSIT;
	nextev->data.deposit.account=account;
	nextev->data.deposit.amount=amount;
	#ifdef EVENTDEBUG
	nextev->head.id=eventid++;
	#endif
	commit_event();
}
void record_withdrawal_event(uint64_t account, uint64_t amount)
{
	if(do_print)
	{
		printf("WITHDRAWAL: %lu from %lu\n", amount, account);
	}
	event* nextev = get_next_event_place();
	nextev->head.kind=EV_WITHDRAW;
	nextev->data.withdraw.account=account;
	nextev->data.withdraw.amount=amount;
	#ifdef EVENTDEBUG
	nextev->head.id=eventid++;
	#endif
	commit_event();
}
void record_failed_withdrawal_event(uint64_t account, uint64_t amount)
{
	if(do_print)
	{
		printf("FAILED WITHDRAWAL: %lu from %lu\n", amount, account);
	}
	event* nextev = get_next_event_place();
	nextev->head.kind=EV_WITHDRAW_FAIL;
	nextev->data.withdraw.account=account;
	nextev->data.withdraw.amount=amount;
	#ifdef EVENTDEBUG
	nextev->head.id=eventid++;
	#endif
	commit_event();
}
void record_transfer_event(uint64_t account, uint64_t target, uint64_t amount)
{
	if(do_print)
	{
		printf("TRANSFER: %lu from %lu to %lu\n", amount, account, target);
	}
	event* nextev = get_next_event_place();
	nextev->head.kind=EV_TRANSFER;
	nextev->data.transfer.account=account;
	nextev->data.transfer.amount=amount;
	nextev->data.transfer.target=target;
	#ifdef EVENTDEBUG
	nextev->head.id=eventid++;
	#endif
	commit_event();
}
void record_failed_transfer_event(uint64_t account, uint64_t target, uint64_t amount)
{
	if(do_print)
	{
		printf("FAILED TRANSFER: %lu from %lu to %lu\n", amount, account, target);
	}
	event* nextev = get_next_event_place();
	nextev->head.kind=EV_TRANSFER_FAIL;
	nextev->data.transfer.account=account;
	nextev->data.transfer.amount=amount;
	nextev->data.transfer.target=target;
	#ifdef EVENTDEBUG
	nextev->head.id=eventid++;
	#endif
	commit_event();
}
void record_checkbalance_event(uint64_t account, uint64_t amount)
{
	if(do_print)
	{
		printf("BALANCE: %lu in %lu\n", amount, account);
	}
	event* nextev = get_next_event_place();
	nextev->head.kind=EV_CHECKBALANCE;
	nextev->data.checkbalance.account=account;
	nextev->data.checkbalance.amount=amount;
	#ifdef EVENTDEBUG
	nextev->head.id=eventid++;
	#endif
	commit_event();
}


uint64_t errors_found=0;
FILE* evprocesslog=NULL;

int process_events_thread(void *arg)
{
	Z3_context ctx = mk_context();
    Z3_solver s = mk_solver(ctx);
	Z3_sort intsort = Z3_mk_int_sort(ctx);
	Z3_symbol worldsortname = Z3_mk_string_symbol(ctx, "World");
	Z3_sort worldsort = Z3_mk_uninterpreted_sort(ctx,worldsortname);
	Z3_symbol accountsortname = Z3_mk_string_symbol(ctx, "Account");
	Z3_sort accountsort = Z3_mk_uninterpreted_sort(ctx,accountsortname);
	Z3_symbol accidname = Z3_mk_string_symbol(ctx, "accid");
	Z3_symbol balancename = Z3_mk_string_symbol(ctx, "balance");
	Z3_symbol worldidname = Z3_mk_string_symbol(ctx, "worldid");
	Z3_sort accid_domain[1] = {accountsort};
	Z3_sort balance_domain[2] = {accountsort, worldsort};
	Z3_func_decl accidfunc = Z3_mk_func_decl(ctx, accidname, 1, accid_domain, intsort);
	Z3_func_decl balancefunc = Z3_mk_func_decl(ctx, balancename, 2, balance_domain, intsort);
	Z3_func_decl worldidfunc = Z3_mk_func_decl(ctx, worldidname, 1, &worldsort, intsort);

	Z3_ast intbound0 = Z3_mk_bound(ctx, 0, intsort);
	Z3_ast accountbound0 = Z3_mk_bound(ctx, 0, accountsort);
	Z3_ast accountbound1 = Z3_mk_bound(ctx, 1, accountsort);
	Z3_ast worldbound0 = Z3_mk_bound(ctx, 0, worldsort);
	Z3_ast worldbound1 = Z3_mk_bound(ctx, 1, worldsort);
	Z3_ast worldbound2 = Z3_mk_bound(ctx, 2, worldsort);

	Z3_ast accid0 = Z3_mk_app(ctx, accidfunc, 1, &accountbound0);
	Z3_ast accid1 = Z3_mk_app(ctx, accidfunc, 1, &accountbound1);
	Z3_ast accid0_1[2] = {accid0, accid1};
	Z3_pattern accid0_1_pattern = Z3_mk_pattern(ctx, 2, accid0_1);

	Z3_sort accounts2[2] = {accountsort, accountsort};
	Z3_symbol acc1sym = Z3_mk_string_symbol(ctx, "acc1");
	Z3_symbol acc2sym = Z3_mk_string_symbol(ctx, "acc2");
	Z3_symbol accountsyms2[2] = {acc2sym, acc1sym};

	Z3_ast accid0_1_eq = Z3_mk_eq(ctx, accid0, accid1);
	Z3_ast acc0_1_eq = Z3_mk_eq(ctx, accountbound0, accountbound1);

	Z3_ast accids_eq_acc_eq = Z3_mk_implies(ctx, accid0_1_eq, acc0_1_eq);
	Z3_ast accids_eq_acc_eq_forall = Z3_mk_forall(ctx, 0, /*1, &accid0_1_pattern,*/ 0,0, 2, accounts2, accountsyms2, accids_eq_acc_eq);
	Z3_solver_assert(ctx, s, accids_eq_acc_eq_forall);

	Z3_ast worldid0 = Z3_mk_app(ctx, worldidfunc, 1, &worldbound0);
	Z3_ast worldid1 = Z3_mk_app(ctx, worldidfunc, 1, &worldbound1);
	Z3_ast worldid2 = Z3_mk_app(ctx, worldidfunc, 1, &worldbound2);
	Z3_ast worldid0_1[2] = {worldid0, worldid1};
	Z3_pattern worldid0_1_pattern = Z3_mk_pattern(ctx, 2, worldid0_1);

	Z3_sort worlds2[2] = {worldsort, worldsort};
	Z3_symbol world1sym = Z3_mk_string_symbol(ctx, "world1");
	Z3_symbol world2sym = Z3_mk_string_symbol(ctx, "world2");
	Z3_symbol worldsyms2[2] = {world2sym, world1sym};

	Z3_ast worldid0_1_eq = Z3_mk_eq(ctx, worldid0, worldid1);
	Z3_ast world0_1_eq = Z3_mk_eq(ctx, worldbound0, worldbound1);

	Z3_ast worldids_eq_acc_eq = Z3_mk_implies(ctx, worldid0_1_eq, world0_1_eq);
	Z3_ast worldids_eq_acc_eq_forall = Z3_mk_forall(ctx, 0, /*1, &worldid0_1_pattern,*/ 0,0, 2, worlds2, worldsyms2, worldids_eq_acc_eq);
	Z3_solver_assert(ctx, s, worldids_eq_acc_eq_forall);

	Z3_ast acc_0_world_1[2] = {accountbound0, worldbound1};
	Z3_ast accbalance0_1 = Z3_mk_app(ctx, balancefunc, 2, acc_0_world_1);
	Z3_ast accbalance_ge_0 = Z3_mk_ge(ctx, accbalance0_1, Z3_mk_int(ctx, 0, intsort));

	Z3_pattern accbalance_pattern = Z3_mk_pattern(ctx, 1, &accbalance0_1);
	Z3_sort account_world_sorts[2] = {worldsort, accountsort};
	Z3_symbol accsym = Z3_mk_string_symbol(ctx, "acc");
	Z3_symbol worldsym = Z3_mk_string_symbol(ctx, "world");
	Z3_symbol accworldsym2[2] = {worldsym, accsym};
	Z3_ast balance_gt_0_forall = Z3_mk_forall(ctx, 0, /*1, &accbalance_pattern,*/ 0,0, 2, account_world_sorts, accworldsym2, accbalance_ge_0);
	Z3_solver_assert(ctx, s, balance_gt_0_forall);

	Z3_symbol initworldname = Z3_mk_string_symbol(ctx, "initworld");
	Z3_ast initworld = Z3_mk_const(ctx, initworldname, worldsort);
	Z3_ast initworldid = Z3_mk_app(ctx, worldidfunc, 1, &initworld);
	Z3_ast initworldid_eq_0 = Z3_mk_eq(ctx, initworldid, Z3_mk_int(ctx, 0, intsort));
	Z3_solver_assert(ctx, s, initworldid_eq_0);

	Z3_solver_push(ctx, s);

	int current_evpos=0;
	int worlds=0;

	int remaining=atomic_load_explicit(&evcount,memory_order_acquire);
	while(monitoring_active)
	{
		if(remaining<=0)
		{
			mtx_lock(&evwait_mutex);
			remaining=atomic_load_explicit(&evcount,memory_order_acquire);
			while(remaining<=0&&monitoring_active)
			{
				cnd_wait(&evwait_cond, &evwait_mutex);
				remaining=atomic_load_explicit(&evcount,memory_order_acquire);
			}
			mtx_unlock(&evwait_mutex);
		}
		int nextbarrier = ((current_evpos/EVBUFSIZE)+1)*EVBUFSIZE;
		if(current_evpos+remaining>nextbarrier)
		{
			remaining=nextbarrier-current_evpos;
		}
		int processed=0;
		while(remaining>0)
		{
			event current=evbuf[current_evpos];
			if(evprocesslog!=NULL)
			{
				printevent(evprocesslog, &current);
			}
			switch(current.head.kind)
			{
				case EV_HOLE:
				{
					if(do_print)
					{
						printf("PROCESSING HOLE\n");
					}
					Z3_solver_pop(ctx, s, 1);
					Z3_solver_push(ctx, s);
					worlds=0;
				}
				break;
				case EV_CHECKBALANCE:
				{
					if(do_print)
					{
						printf("PROCESSING CHECKBALANCE\n");
					}
					Z3_ast currentaccountid = Z3_mk_int(ctx, current.data.checkbalance.account, intsort);
					Z3_ast worldid_past = Z3_mk_eq(ctx, worldid0, Z3_mk_int(ctx, worlds, intsort));
					worlds++;
					Z3_ast worldid_new = Z3_mk_eq(ctx, worldid1, Z3_mk_int(ctx, worlds, intsort));
					
					Z3_ast accbalance_old_args[2] = {accountbound0, worldbound1};
					Z3_ast accbalance_new_args[2] = {accountbound0, worldbound2};
					Z3_ast accbalance_old = Z3_mk_app(ctx, balancefunc, 2, accbalance_old_args);
					Z3_ast accbalance_new = Z3_mk_app(ctx, balancefunc, 2, accbalance_new_args);

					Z3_ast balances_eq = Z3_mk_eq(ctx, accbalance_old, accbalance_new);
					Z3_ast balance_check = Z3_mk_implies(ctx, Z3_mk_eq(ctx, accid0, currentaccountid), Z3_mk_eq(ctx, accbalance_new, Z3_mk_int(ctx, current.data.checkbalance.amount, intsort)));
					Z3_ast forall_ands[2] = {balances_eq, balance_check};
					Z3_ast forall_body = Z3_mk_and(ctx, 2, forall_ands);
					Z3_ast balances_forall = Z3_mk_forall(ctx, 0, 0, NULL, 1, &accountsort, &accsym, forall_body);

					Z3_ast ex_ands[3] = {worldid_past, worldid_new, balances_forall};
					Z3_ast ex_body = Z3_mk_and(ctx, 3, ex_ands);
					Z3_ast exist_worlds = Z3_mk_exists(ctx, 0, 0, NULL, 2, worlds2, worldsyms2, ex_body);
					Z3_solver_assert(ctx, s, exist_worlds);

					Z3_ast exists_account = Z3_mk_exists(ctx, 0, 0, 0, 1, &accountsort, &accsym, Z3_mk_eq(ctx, Z3_mk_app(ctx, accidfunc, 1, &accountbound0), currentaccountid));
					Z3_solver_assert(ctx, s, exists_account);

					if(Z3_solver_check(ctx, s)==Z3_L_FALSE)
					{
						errors_found++;
						if(do_print_unsat_smt)
						{
							printf("%s\n",Z3_solver_to_string(ctx, s));
						}
						Z3_solver_pop(ctx, s, 1);
						Z3_solver_push(ctx, s);
						printf("Mismatch found at checkbalance event: %lu in %lu\n", current.data.checkbalance.amount, current.data.checkbalance.account);
						worlds=0;
					}
					else if(do_print_smt)
					{
						Z3_model model = Z3_solver_get_model(ctx, s);
						printf("%s\n",Z3_model_to_string(ctx, model));
						printf("%s\n",Z3_solver_to_string(ctx, s));
					}
				}
				break;
				case EV_DEPOSIT:
				{
					if(do_print)
					{
						printf("PROCESSING DEPOSIT\n");
					}
					Z3_ast currentaccountid = Z3_mk_int(ctx, current.data.deposit.account, intsort);
					Z3_ast worldid_past = Z3_mk_eq(ctx, worldid0, Z3_mk_int(ctx, worlds, intsort));
					worlds++;
					Z3_ast worldid_new = Z3_mk_eq(ctx, worldid1, Z3_mk_int(ctx, worlds, intsort));
					
					Z3_ast accbalance_old_args[2] = {accountbound0, worldbound1};
					Z3_ast accbalance_new_args[2] = {accountbound0, worldbound2};
					Z3_ast accbalance_old = Z3_mk_app(ctx, balancefunc, 2, accbalance_old_args);
					Z3_ast accbalance_new = Z3_mk_app(ctx, balancefunc, 2, accbalance_new_args);

					Z3_ast balances_eq = Z3_mk_eq(ctx, accbalance_old, accbalance_new);
					Z3_ast add_args[2] = {accbalance_old, Z3_mk_int(ctx, current.data.deposit.amount, intsort)};
					Z3_ast and_args[2] = {Z3_mk_eq(ctx, accid0, currentaccountid), Z3_mk_eq(ctx, accbalance_new, Z3_mk_add(ctx, 2, add_args))};
					Z3_ast balance_add = Z3_mk_and(ctx, 2, and_args);
					Z3_ast forall_ors[2] = {balances_eq, balance_add};
					Z3_ast forall_body = Z3_mk_or(ctx, 2, forall_ors);
					Z3_ast balances_forall = Z3_mk_forall(ctx, 0, 0, NULL, 1, &accountsort, &accsym, forall_body);

					Z3_ast ex_ands[3] = {worldid_past, worldid_new, balances_forall};
					Z3_ast ex_body = Z3_mk_and(ctx, 3, ex_ands);
					Z3_ast exist_worlds = Z3_mk_exists(ctx, 0, 0, NULL, 2, worlds2, worldsyms2, ex_body);
					Z3_solver_assert(ctx, s, exist_worlds);

					Z3_ast exists_account = Z3_mk_exists(ctx, 0, 0, 0, 1, &accountsort, &accsym, Z3_mk_eq(ctx, Z3_mk_app(ctx, accidfunc, 1, &accountbound0), currentaccountid));
					Z3_solver_assert(ctx, s, exists_account);

					if(Z3_solver_check(ctx, s)==Z3_L_FALSE)
					{
						errors_found++;
						if(do_print_unsat_smt)
						{
							printf("%s\n",Z3_solver_to_string(ctx, s));
						}
						printf("Mismatch found at deposit event: %lu to %lu\n", current.data.deposit.amount, current.data.deposit.account);
						Z3_solver_pop(ctx, s, 1);
						Z3_solver_push(ctx, s);
						worlds=0;
					}
					else if(do_print_smt)
					{
						Z3_model model = Z3_solver_get_model(ctx, s);
						printf("%s\n",Z3_model_to_string(ctx, model));
						printf("%s\n",Z3_solver_to_string(ctx, s));
					}
				}
				break;
				case EV_WITHDRAW:
				{
					if(do_print)
					{
						printf("PROCESSING WITHDRAWAL\n");
					}
					Z3_ast currentaccountid = Z3_mk_int(ctx, current.data.withdraw.account, intsort);
					Z3_ast worldid_past = Z3_mk_eq(ctx, worldid0, Z3_mk_int(ctx, worlds, intsort));
					worlds++;
					Z3_ast worldid_new = Z3_mk_eq(ctx, worldid1, Z3_mk_int(ctx, worlds, intsort));
					
					Z3_ast accbalance_old_args[2] = {accountbound0, worldbound1};
					Z3_ast accbalance_new_args[2] = {accountbound0, worldbound2};
					Z3_ast accbalance_old = Z3_mk_app(ctx, balancefunc, 2, accbalance_old_args);
					Z3_ast accbalance_new = Z3_mk_app(ctx, balancefunc, 2, accbalance_new_args);

					Z3_ast balances_eq = Z3_mk_eq(ctx, accbalance_old, accbalance_new);
					Z3_ast sub_args[2] = {accbalance_old, Z3_mk_int(ctx, current.data.withdraw.amount, intsort)};
					Z3_ast and_args[2] = {Z3_mk_eq(ctx, accid0, currentaccountid), Z3_mk_eq(ctx, accbalance_new, Z3_mk_sub(ctx, 2, sub_args))};
					Z3_ast balance_add = Z3_mk_and(ctx, 2, and_args);
					Z3_ast forall_ors[2] = {balances_eq, balance_add};
					Z3_ast forall_body = Z3_mk_or(ctx, 2, forall_ors);
					Z3_ast balances_forall = Z3_mk_forall(ctx, 0, 0, NULL, 1, &accountsort, &accsym, forall_body);

					Z3_ast ex_ands[3] = {worldid_past, worldid_new, balances_forall};
					Z3_ast ex_body = Z3_mk_and(ctx, 3, ex_ands);
					Z3_ast exist_worlds = Z3_mk_exists(ctx, 0, 0, NULL, 2, worlds2, worldsyms2, ex_body);
					Z3_solver_assert(ctx, s, exist_worlds);

					Z3_ast exists_account = Z3_mk_exists(ctx, 0, 0, 0, 1, &accountsort, &accsym, Z3_mk_eq(ctx, Z3_mk_app(ctx, accidfunc, 1, &accountbound0), currentaccountid));
					Z3_solver_assert(ctx, s, exists_account);

					if(Z3_solver_check(ctx, s)==Z3_L_FALSE)
					{
						errors_found++;
						if(do_print_unsat_smt)
						{
							printf("%s\n",Z3_solver_to_string(ctx, s));
						}
						printf("Mismatch found at withdraw event: %lu to %lu\n", current.data.withdraw.amount, current.data.withdraw.account);
						Z3_solver_pop(ctx, s, 1);
						Z3_solver_push(ctx, s);
						worlds=0;
					}
					else if(do_print_smt)
					{
						Z3_model model = Z3_solver_get_model(ctx, s);
						printf("%s\n",Z3_model_to_string(ctx, model));
						printf("%s\n",Z3_solver_to_string(ctx, s));
					}
				}
				break;
				case EV_WITHDRAW_FAIL:
				{
					if(do_print)
					{
						printf("PROCESSING FAILED WITHDRAWAL\n");
					}
					Z3_ast currentaccountid = Z3_mk_int(ctx, current.data.withdraw.account, intsort);
					Z3_ast worldid_past = Z3_mk_eq(ctx, worldid0, Z3_mk_int(ctx, worlds, intsort));
					worlds++;
					Z3_ast worldid_new = Z3_mk_eq(ctx, worldid1, Z3_mk_int(ctx, worlds, intsort));
					
					Z3_ast accbalance_old_args[2] = {accountbound0, worldbound1};
					Z3_ast accbalance_new_args[2] = {accountbound0, worldbound2};
					Z3_ast accbalance_old = Z3_mk_app(ctx, balancefunc, 2, accbalance_old_args);
					Z3_ast accbalance_new = Z3_mk_app(ctx, balancefunc, 2, accbalance_new_args);

					Z3_ast balances_eq = Z3_mk_eq(ctx, accbalance_old, accbalance_new);
					Z3_ast balance_check = Z3_mk_implies(ctx, Z3_mk_eq(ctx, accid0, currentaccountid), Z3_mk_lt(ctx, accbalance_new, Z3_mk_int(ctx, current.data.withdraw.amount, intsort)));
					Z3_ast forall_ands[2] = {balances_eq, balance_check};
					Z3_ast forall_body = Z3_mk_and(ctx, 2, forall_ands);
					Z3_ast balances_forall = Z3_mk_forall(ctx, 0, 0, NULL, 1, &accountsort, &accsym, forall_body);

					Z3_ast ex_ands[3] = {worldid_past, worldid_new, balances_forall};
					Z3_ast ex_body = Z3_mk_and(ctx, 3, ex_ands);
					Z3_ast exist_worlds = Z3_mk_exists(ctx, 0, 0, NULL, 2, worlds2, worldsyms2, ex_body);
					Z3_solver_assert(ctx, s, exist_worlds);
					
					Z3_ast exists_account = Z3_mk_exists(ctx, 0, 0, 0, 1, &accountsort, &accsym, Z3_mk_eq(ctx, Z3_mk_app(ctx, accidfunc, 1, &accountbound0), currentaccountid));
					Z3_solver_assert(ctx, s, exists_account);

					if(Z3_solver_check(ctx, s)==Z3_L_FALSE)
					{
						errors_found++;
						if(do_print_unsat_smt)
						{
							printf("%s\n",Z3_solver_to_string(ctx, s));
						}
						Z3_solver_pop(ctx, s, 1);
						Z3_solver_push(ctx, s);
						printf("Mismatch found at failed withdraw event: %lu from %lu\n", current.data.checkbalance.amount, current.data.checkbalance.account);
						worlds=0;
					}
					else if(do_print_smt)
					{
						Z3_model model = Z3_solver_get_model(ctx, s);
						printf("%s\n",Z3_model_to_string(ctx, model));
						printf("%s\n",Z3_solver_to_string(ctx, s));
					}
				}
				break;
				case EV_TRANSFER:
				{
					if(do_print)
					{
						printf("PROCESSING TRANSFER\n");
					}
					Z3_ast currentaccountid = Z3_mk_int(ctx, current.data.transfer.account, intsort);
					Z3_ast targetaccountid = Z3_mk_int(ctx, current.data.transfer.account, intsort);
					Z3_ast worldid_past = Z3_mk_eq(ctx, worldid0, Z3_mk_int(ctx, worlds, intsort));
					worlds++;
					Z3_ast worldid_new = Z3_mk_eq(ctx, worldid1, Z3_mk_int(ctx, worlds, intsort));
					
					Z3_ast accbalance_old_args[2] = {accountbound0, worldbound1};
					Z3_ast accbalance_new_args[2] = {accountbound0, worldbound2};
					Z3_ast accbalance_old = Z3_mk_app(ctx, balancefunc, 2, accbalance_old_args);
					Z3_ast accbalance_new = Z3_mk_app(ctx, balancefunc, 2, accbalance_new_args);

					Z3_ast balances_eq = Z3_mk_eq(ctx, accbalance_old, accbalance_new);
					Z3_ast sub_args[2] = {accbalance_old, Z3_mk_int(ctx, current.data.transfer.amount, intsort)};
					Z3_ast and_args[2] = {Z3_mk_eq(ctx, accid0, currentaccountid), Z3_mk_eq(ctx, accbalance_new, Z3_mk_sub(ctx, 2, sub_args))};
					Z3_ast add_args[2] = {accbalance_old, Z3_mk_int(ctx, current.data.transfer.amount, intsort)};
					Z3_ast and_args_target[2] = {Z3_mk_eq(ctx, accid0, targetaccountid), Z3_mk_eq(ctx, accbalance_new, Z3_mk_add(ctx, 2, add_args))};
					Z3_ast balance_add = Z3_mk_and(ctx, 2, and_args);
					Z3_ast balance_add_target = Z3_mk_and(ctx, 2, and_args_target);
					
					Z3_ast forall_ors[3] = {balances_eq, balance_add, balance_add_target};
					Z3_ast forall_body = Z3_mk_or(ctx, 3, forall_ors);
					Z3_ast balances_forall = Z3_mk_forall(ctx, 0, 0, NULL, 1, &accountsort, &accsym, forall_body);

					Z3_ast ex_ands[3] = {worldid_past, worldid_new, balances_forall};
					Z3_ast ex_body = Z3_mk_and(ctx, 3, ex_ands);
					Z3_ast exist_worlds = Z3_mk_exists(ctx, 0, 0, NULL, 2, worlds2, worldsyms2, ex_body);
					Z3_solver_assert(ctx, s, exist_worlds);

					Z3_ast exists_account = Z3_mk_exists(ctx, 0, 0, 0, 1, &accountsort, &accsym, Z3_mk_eq(ctx, Z3_mk_app(ctx, accidfunc, 1, &accountbound0), currentaccountid));
					Z3_solver_assert(ctx, s, exists_account);

					Z3_ast exists_target_account = Z3_mk_exists(ctx, 0, 0, 0, 1, &accountsort, &accsym, Z3_mk_eq(ctx, Z3_mk_app(ctx, accidfunc, 1, &accountbound0), targetaccountid));
					Z3_solver_assert(ctx, s, exists_target_account);

					if(Z3_solver_check(ctx, s)==Z3_L_FALSE)
					{
						errors_found++;
						if(do_print_unsat_smt)
						{
							printf("%s\n",Z3_solver_to_string(ctx, s));
						}
						printf("Mismatch found at transfer event: %lu from %lu to %lu\n", current.data.transfer.amount, current.data.transfer.account, current.data.transfer.target);
						Z3_solver_pop(ctx, s, 1);
						Z3_solver_push(ctx, s);
						worlds=0;
					}
					else if(do_print_smt)
					{
						Z3_model model = Z3_solver_get_model(ctx, s);
						printf("%s\n",Z3_model_to_string(ctx, model));
						printf("%s\n",Z3_solver_to_string(ctx, s));
					}
				}
				break;
				case EV_TRANSFER_FAIL:
				{
					if(do_print)
					{
						printf("PROCESSING FAILED TRANSFER\n");
					}
					Z3_ast currentaccountid = Z3_mk_int(ctx, current.data.transfer.account, intsort);
					Z3_ast targetaccountid = Z3_mk_int(ctx, current.data.transfer.account, intsort);
					Z3_ast worldid_past = Z3_mk_eq(ctx, worldid0, Z3_mk_int(ctx, worlds, intsort));
					worlds++;
					Z3_ast worldid_new = Z3_mk_eq(ctx, worldid1, Z3_mk_int(ctx, worlds, intsort));
					
					Z3_ast accbalance_old_args[2] = {accountbound0, worldbound1};
					Z3_ast accbalance_new_args[2] = {accountbound0, worldbound2};
					Z3_ast accbalance_old = Z3_mk_app(ctx, balancefunc, 2, accbalance_old_args);
					Z3_ast accbalance_new = Z3_mk_app(ctx, balancefunc, 2, accbalance_new_args);

					Z3_ast balances_eq = Z3_mk_eq(ctx, accbalance_old, accbalance_new);
					Z3_ast balance_check = Z3_mk_implies(ctx, Z3_mk_eq(ctx, accid0, currentaccountid), Z3_mk_lt(ctx, accbalance_new, Z3_mk_int(ctx, current.data.transfer.amount, intsort)));
					Z3_ast forall_ands[2] = {balances_eq, balance_check};
					Z3_ast forall_body = Z3_mk_and(ctx, 2, forall_ands);
					Z3_ast balances_forall = Z3_mk_forall(ctx, 0, 0, NULL, 1, &accountsort, &accsym, forall_body);

					Z3_ast ex_ands[3] = {worldid_past, worldid_new, balances_forall};
					Z3_ast ex_body = Z3_mk_and(ctx, 3, ex_ands);
					Z3_ast exist_worlds = Z3_mk_exists(ctx, 0, 0, NULL, 2, worlds2, worldsyms2, ex_body);
					Z3_solver_assert(ctx, s, exist_worlds);

					Z3_ast exists_account = Z3_mk_exists(ctx, 0, 0, 0, 1, &accountsort, &accsym, Z3_mk_eq(ctx, Z3_mk_app(ctx, accidfunc, 1, &accountbound0), currentaccountid));
					Z3_solver_assert(ctx, s, exists_account);
					
					Z3_ast exists_target_account = Z3_mk_exists(ctx, 0, 0, 0, 1, &accountsort, &accsym, Z3_mk_eq(ctx, Z3_mk_app(ctx, accidfunc, 1, &accountbound0), targetaccountid));
					Z3_solver_assert(ctx, s, exists_target_account);

					if(Z3_solver_check(ctx, s)==Z3_L_FALSE)
					{
						errors_found++;
						if(do_print_unsat_smt)
						{
							printf("%s\n",Z3_solver_to_string(ctx, s));
						}
						Z3_solver_pop(ctx, s, 1);
						Z3_solver_push(ctx, s);
						printf("Mismatch found at failed transfer event: %lu from %lu\n", current.data.checkbalance.amount, current.data.checkbalance.account);
						worlds=0;
					}
					else if(do_print_smt)
					{
						Z3_model model = Z3_solver_get_model(ctx, s);
						printf("%s\n",Z3_model_to_string(ctx, model));
						printf("%s\n",Z3_solver_to_string(ctx, s));
					}
				}
				break;
			}
			int newpos=(current_evpos+1)%ACTUALEVBUFSIZE;
			processed++;
			if(atomic_compare_exchange_strong(&evpos, &current_evpos, newpos))
			{
				current_evpos=newpos;
				remaining--;
			}
			else
			{
				if(newpos>current_evpos)
				{
					processed+=ACTUALEVBUFSIZE-newpos;
					newpos=0;					
				}
				processed+=current_evpos-newpos;
				if(evprocesslog!=NULL)
				{
					fprintf(evprocesslog, "Skipping ahead in process log to %i\n", current_evpos);
				}
				break;
			}
		}
		remaining=atomic_fetch_sub_explicit(&evcount, processed, memory_order_acq_rel)-processed;
	}
}

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

int buf_advance_past(msgbuf *buf, char c)
{
	if(buf->textbuf==NULL)
	{
		return 0;
	}
	msgbuf *current=buf;
	do
	{
		char *found = strchr(current->textbuf, c);
		if(found==NULL)
		{
			free(current->textbuf-current->offset);
			current->textbuf=NULL;
			current->offset=0;
			if(current!=buf)
			{
				msgbuf* old=current;
				current=current->next;
				free(old);
			}
			else
			{
				current=current->next;
			}
		}
		else
		{
			current->offset+=found-current->textbuf+1;
			current->textbuf=found+1;
			if(strlen(current->textbuf)==0)
			{
				free(current->textbuf-current->offset);
				current->textbuf=NULL;
				current->offset=0;
				if(current!=buf)
				{
					msgbuf* old=current;
					current=current->next;
					free(old);
				}
			}
			if(current!=buf)
			{
				buf->textbuf=current->textbuf;
				buf->offset=current->offset;
				buf->next=current->next;
				buf->next->prev=buf;
				free(current);
			}
			else
			{
				buf->next=buf;
				buf->prev=buf;
			}
			return 1;
		}
	} while(current!=buf);
	buf->next=buf;
	buf->prev=buf;
	buf->textbuf=NULL;
	buf->offset=0;
	return 0;
}

char * buf_match_get_until(msgbuf *buf, char* prefix, char end)
{
	if(buf->textbuf==NULL)
	{
		return prefix;
	}
	msgbuf* current=buf;
	size_t size=0;
	int prefix_left=strlen(prefix);
	char* prefix_orig=prefix;
	size_t pos_in_current=0;
	while(prefix_left>0)
	{
		int tblen=strlen(current->textbuf);
		int cmplen=prefix_left;
		if(tblen<prefix_left)
		{
			cmplen=tblen;
		}
		if(strncmp(current->textbuf, prefix, cmplen)==0)
		{
			prefix_left-=cmplen;
			prefix+=cmplen;
			size+=cmplen;
			if(prefix_left>0)
			{
				current=current->next;
				if(current==buf)
				{
					return prefix_orig; //indicates that prefix matches so far, but no more data available; try again later
				}
			}
			else
			{
				pos_in_current=cmplen;
			}
		}
		else
		{
			return NULL; //mismatch found
		}
	}
	//prefix fully matched, now look for end
	do
	{
		char* found=strchr(current->textbuf, end);
		if(found!=NULL)
		{
			msgbuf* foundbuf=current;
			size_t cursize = (found-current->textbuf)+1;
			size+=cursize-pos_in_current;
			char* ret = (char*)malloc(size+1);
			char* curend = ret+size;
			curend-=cursize;
			memcpy(curend, current->textbuf, cursize);
			foundbuf->textbuf+=cursize;
			foundbuf->offset+=cursize;
			while(current!=buf)
			{
				msgbuf* old=current;
				current=current->prev;
				if(old!=foundbuf)
				{
					free(old->textbuf-old->offset);
					free(old);
				}
				cursize=strlen(current->textbuf);
				curend-=cursize;
				memcpy(curend, current->textbuf, cursize);
			}
			if(foundbuf!=buf)
			{
				free(buf->textbuf-buf->offset);
				buf->textbuf=NULL;
				buf->offset=0;
				if(strlen(foundbuf->textbuf)>0)
				{
					buf->textbuf=foundbuf->textbuf;
					buf->offset=foundbuf->offset;
					buf->next=foundbuf->next;
					buf->next->prev=buf;
					free(foundbuf);
				}
				else
				{
					free(foundbuf->textbuf-foundbuf->offset);
					if(foundbuf->next!=buf)
					{
						buf->textbuf=foundbuf->next->textbuf;
						buf->offset=foundbuf->next->offset;
						buf->next=foundbuf->next->next;
						free(foundbuf->next);
					}
					else
					{
						buf->next=buf;
					}
					buf->next->prev=buf;
					free(foundbuf);
				}
			}
			else if(strlen(buf->textbuf)==0)
			{
				free(buf->textbuf-buf->offset);
				buf->textbuf=NULL;
				buf->offset=0;
				if(buf->next!=buf)
				{
					msgbuf* old=buf->next;
					buf->textbuf=old->textbuf;
					buf->offset=old->offset;
					buf->next=old->next;
					buf->next->prev=buf;
					free(old);
				}
			}
			ret[size]=0;
			return ret;
		}
		size+=strlen(current->textbuf)-pos_in_current;
		pos_in_current=0;
		current=current->next;
	} while (current!=buf);
	return prefix;
}

typedef enum _processing_state { PS_NOTHING, PS_NOTHING_RESPONSE, PS_DEPOSIT, PS_WITHDRAWAL, PS_WITHDRAWAL_RESULT, PS_WITHDRAWAL_RESULT_LINE2, PS_TRANSFER, PS_TRANSFER_TARGET, PS_TRANSFER_AMOUNT, PS_TRANSFER_RESULT, PS_TRANSFER_RESULT_LINE2, PS_CHECKBALANCE } processing_state;

void process_messages(msgbuf *writebuf, msgbuf *readbuf)
{
	static processing_state current_state=PS_NOTHING;
	static char * pfx_deposit = "\e[1;1H\e[2JDeposit";
	static char * pfx_withdraw = "\e[1;1H\e[2JWithdraw";
	static char * pfx_transfer = "\e[1;1H\e[2JTransfer";
	static char * pfx_checkbalance = "\e[1;1H\e[2JCurrent balance";
	static char * pfx_emptystr = "";
	static long account=0;
	static long amount=0;
	static long target=0;
	char* endptr;
	static int newpage=0;
	while(1)
	{
		if(newpage)
		{
			if(!buf_advance_past(writebuf,'>'))
			{
				return;
			}
			newpage=0;
		}
		if(current_state==PS_NOTHING)
		{
			char* firstline = buf_match_get_until(writebuf, pfx_deposit, '\n');
			if(firstline==pfx_deposit)
			{
				return;
			}
			else if(firstline!=NULL)
			{
				account=strtoull(firstline+29, &endptr, 10);
				current_state=PS_DEPOSIT;
				free(firstline);
				continue;
			}
			firstline = buf_match_get_until(writebuf, pfx_withdraw, '\n');
			if(firstline==pfx_withdraw)
			{
				return;
			}
			else if(firstline!=NULL)
			{
				account=strtoull(firstline+32, &endptr, 10);
				current_state=PS_WITHDRAWAL;
				free(firstline);
				continue;
			}
			firstline = buf_match_get_until(writebuf, pfx_transfer, '\n');
			if(firstline==pfx_transfer)
			{
				return;
			}
			else if(firstline!=NULL)
			{
				account=strtoull(firstline+32, &endptr, 10);
				current_state=PS_TRANSFER;
				free(firstline);
				continue;
			}
			firstline = buf_match_get_until(writebuf, pfx_checkbalance, '\n');
			if(firstline==pfx_checkbalance)
			{
				return;
			}
			else if(firstline!=NULL)
			{
				account=strtoull(firstline+36, &endptr, 10);
				current_state=PS_CHECKBALANCE;
				free(firstline);
				continue;
			}
			current_state=PS_NOTHING_RESPONSE;
			continue;
		}
		else if(current_state==PS_NOTHING_RESPONSE)
		{
			if(!buf_advance_past(readbuf,'\n'))
			{
				return;
			}
			current_state=PS_NOTHING;
			newpage=1;
			continue;
		}
		else if(current_state==PS_CHECKBALANCE)
		{
			char* line = buf_match_get_until(writebuf, pfx_emptystr, '\n');
			if(line==pfx_emptystr)
			{
				return;
			}
			amount=strtoull(line,&endptr, 10);
			record_checkbalance_event(account,amount);
			current_state=PS_NOTHING_RESPONSE;
			free(line);
			continue;
		}
		else if(current_state==PS_DEPOSIT)
		{
			char* line = buf_match_get_until(readbuf, pfx_emptystr, '\n');
			if(line==pfx_emptystr)
			{
				return;
			}
			amount=atol(line);
			if(amount>0)
			{
				record_deposit_event(account,amount);
			}
			current_state=PS_NOTHING;
			newpage=1;
			free(line);
			continue;
		}
		else if(current_state==PS_WITHDRAWAL)
		{
			char* line = buf_match_get_until(readbuf, pfx_emptystr, '\n');
			if(line==pfx_emptystr)
			{
				return;
			}
			amount=atol(line);
			current_state=PS_WITHDRAWAL_RESULT;
			newpage=1;
			free(line);
			continue;
		}
		else if(current_state==PS_WITHDRAWAL_RESULT)
		{
			char* line = buf_match_get_until(writebuf, pfx_emptystr, '\n');
			if(line==pfx_emptystr)
			{
				return;
			}
			current_state=PS_WITHDRAWAL_RESULT_LINE2;
			free(line);
			continue;
		}
		else if(current_state==PS_WITHDRAWAL_RESULT_LINE2)
		{
			char* line = buf_match_get_until(writebuf, pfx_emptystr, '\n');
			if(line==pfx_emptystr)
			{
				return;
			}
			if(strncmp(line,"Withdrawal successful!", 20)==0)
			{
				record_withdrawal_event(account,amount);
			}
			else
			{
				if(amount>0)
				{
					record_failed_withdrawal_event(account, amount);
				}
			}
			free(line);
			current_state=PS_NOTHING_RESPONSE;
			continue;
		}
		else if(current_state==PS_TRANSFER)
		{
			char* line = buf_match_get_until(readbuf, pfx_emptystr, '\n');
			if(line==pfx_emptystr)
			{
				return;
			}
			target=strtoull(line,&endptr, 10);
			current_state=PS_TRANSFER_TARGET;
			newpage=1;
			free(line);
			continue;
		}
		else if(current_state==PS_TRANSFER_TARGET)
		{
			char* firstline = buf_match_get_until(writebuf, pfx_transfer, '\n');
			if(firstline==pfx_transfer)
			{
				return;
			}
			else if(firstline!=NULL)
			{
				current_state=PS_TRANSFER_AMOUNT;
				free(firstline);
				continue;
			}
			current_state=PS_NOTHING_RESPONSE;
		}
		else if(current_state==PS_TRANSFER_AMOUNT)
		{
			char* line = buf_match_get_until(readbuf, pfx_emptystr, '\n');
			if(line==pfx_emptystr)
			{
				return;
			}
			amount=atol(line);
			current_state=PS_TRANSFER_RESULT;
			newpage=1;
			free(line);
			continue;
		}
		else if(current_state==PS_TRANSFER_RESULT)
		{
			char* line = buf_match_get_until(writebuf, pfx_emptystr, '\n');
			if(line==pfx_emptystr)
			{
				return;
			}
			current_state=PS_TRANSFER_RESULT_LINE2;
			free(line);
			continue;
		}
		else if(current_state==PS_TRANSFER_RESULT_LINE2)
		{
			char* line = buf_match_get_until(writebuf, pfx_emptystr, '\n');
			if(line==pfx_emptystr)
			{
				return;
			}
			if(strncmp(line,"Transfer successful!", 20)==0)
			{
				record_transfer_event(account,target,amount);
			}
			else
			{
				if(amount>0)
				{
					record_failed_transfer_event(account, target, amount);
				}
			}
			free(line);
			current_state=PS_NOTHING_RESPONSE;
			continue;
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
			processed_bytes += buffer_buffer[i].payload64_2;
		}
		if(count>0)
		{
			process_messages(&write_msg, &read_msg);
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
	for (int i=1;i<argc-1;i++)
	{
		if (strncmp(argv[i], "print", 6) == 0)
		{
			do_print = 1;
		}
		else if (strncmp(argv[i], "printsmt", 9) == 0)
		{
			do_print_smt = 1;
			do_print=1;
		}
		else if (strncmp(argv[i], "printunsat", 11) == 0)
		{
			do_print_unsat_smt = 1;
		}
		else if (strncmp(argv[i], "debugevents", 12) == 0)
		{
			evparselog=fopen("evparselog.txt", "w+");
			evprocesslog=fopen("evprocesslog.txt", "w+");
		}
	}
	#ifdef EVENTDEBUG
	for(int i=0;i<ACTUALEVBUFSIZE;i++)
	{
		evbuf[i].head.index=i;
	}
	#endif
	pid_t process_id = atoi(argv[argc-1]);
	mtx_init(&evwait_mutex, mtx_plain);
	cnd_init(&evwait_cond);
	atomic_init(&evpos, 0);
	atomic_init(&evcount, 0);
	thrd_t smt_thread;
	thrd_create(&smt_thread, process_events_thread, NULL);
	monitored_process proc = attach_to_process(process_id, &register_monitored_thread);

	wait_for_process(proc);
	monitoring_active = 0;
	printf("Processed bytes: %lu\n", processed_bytes);
	mtx_lock(&evwait_mutex);
	cnd_broadcast(&evwait_cond);
	mtx_unlock(&evwait_mutex);
	thrd_join(smt_thread, NULL);
	printf("Errors found: %lu\n", errors_found);
	if(evparselog!=NULL)
	{
		fflush(evparselog);
		fclose(evparselog);
	}
	if(evprocesslog!=NULL)
	{
		fflush(evprocesslog);
		fclose(evprocesslog);
	}
	return 0;
}
