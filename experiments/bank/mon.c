#include "monitor.h"
#include "./compiler/cfiles/compiler_utils.h"
#include "mmlib.h"
#include "shamon.h"
#include <stdatomic.h>
#include <stdio.h>
#include <threads.h>

struct _EVENT_hole {
    uint64_t n;
};
typedef struct _EVENT_hole EVENT_hole;
// event declarations for stream type BankOutputEvent
struct _EVENT_balance {
    int account;
};
typedef struct _EVENT_balance EVENT_balance;
struct _EVENT_depositTo {
    int account;
};
typedef struct _EVENT_depositTo EVENT_depositTo;
struct _EVENT_withdraw {
    int account;
};
typedef struct _EVENT_withdraw EVENT_withdraw;
struct _EVENT_transfer {
    int fromAccount;
    int toAccount;
};
typedef struct _EVENT_transfer EVENT_transfer;
struct _EVENT_depositSuccess {};
typedef struct _EVENT_depositSuccess EVENT_depositSuccess;
struct _EVENT_withdrawSuccess {};
typedef struct _EVENT_withdrawSuccess EVENT_withdrawSuccess;
struct _EVENT_withdrawFail {};
typedef struct _EVENT_withdrawFail EVENT_withdrawFail;
struct _EVENT_transferSuccess {};
typedef struct _EVENT_transferSuccess EVENT_transferSuccess;
struct _EVENT_selectedAccount {
    int account;
};
typedef struct _EVENT_selectedAccount EVENT_selectedAccount;
struct _EVENT_readInput {};
typedef struct _EVENT_readInput EVENT_readInput;
struct _EVENT_login {};
typedef struct _EVENT_login EVENT_login;
struct _EVENT_numOut {
    int num;
};
typedef struct _EVENT_numOut EVENT_numOut;

// input stream for stream type BankOutputEvent
struct _STREAM_BankOutputEvent_in {
    shm_event head;
    union {
        EVENT_depositSuccess  depositSuccess;
        EVENT_selectedAccount selectedAccount;
        EVENT_depositTo       depositTo;
        EVENT_balance         balance;
        EVENT_readInput       readInput;
        EVENT_transfer        transfer;
        EVENT_transferSuccess transferSuccess;
        EVENT_numOut          numOut;
        EVENT_withdraw        withdraw;
        EVENT_withdrawSuccess withdrawSuccess;
        EVENT_withdrawFail    withdrawFail;
        EVENT_login           login;
    } cases;
};
typedef struct _STREAM_BankOutputEvent_in STREAM_BankOutputEvent_in;

// output stream for stream type BankOutputEvent
struct _STREAM_BankOutputEvent_out {
    shm_event head;
    union {
        EVENT_hole            hole;
        EVENT_depositSuccess  depositSuccess;
        EVENT_selectedAccount selectedAccount;
        EVENT_depositTo       depositTo;
        EVENT_balance         balance;
        EVENT_readInput       readInput;
        EVENT_transfer        transfer;
        EVENT_transferSuccess transferSuccess;
        EVENT_numOut          numOut;
        EVENT_withdraw        withdraw;
        EVENT_withdrawSuccess withdrawSuccess;
        EVENT_withdrawFail    withdrawFail;
        EVENT_login           login;
    } cases;
};
typedef struct _STREAM_BankOutputEvent_out STREAM_BankOutputEvent_out;
// event declarations for stream type BankInputEvent
struct _EVENT_numIn {
    int num;
};
typedef struct _EVENT_numIn EVENT_numIn;
struct _EVENT_otherIn {};
typedef struct _EVENT_otherIn EVENT_otherIn;

// input stream for stream type BankInputEvent
struct _STREAM_BankInputEvent_in {
    shm_event head;
    union {
        EVENT_otherIn otherIn;
        EVENT_numIn   numIn;
    } cases;
};
typedef struct _STREAM_BankInputEvent_in STREAM_BankInputEvent_in;

// output stream for stream type BankInputEvent
struct _STREAM_BankInputEvent_out {
    shm_event head;
    union {
        EVENT_hole    hole;
        EVENT_otherIn otherIn;
        EVENT_numIn   numIn;
    } cases;
};
typedef struct _STREAM_BankInputEvent_out STREAM_BankInputEvent_out;
// event declarations for stream type BankEvent
struct _EVENT_SawBalance {
    int account;
    int balance;
};
typedef struct _EVENT_SawBalance EVENT_SawBalance;
struct _EVENT_SawDeposit {
    int account;
    int amount;
};
typedef struct _EVENT_SawDeposit EVENT_SawDeposit;
struct _EVENT_SawWithdraw {
    int account;
    int amount;
};
typedef struct _EVENT_SawWithdraw EVENT_SawWithdraw;
struct _EVENT_SawWithdrawFail {
    int account;
    int amount;
};
typedef struct _EVENT_SawWithdrawFail EVENT_SawWithdrawFail;
struct _EVENT_SawTransfer {
    int from_;
    int to_;
    int amount;
};
typedef struct _EVENT_SawTransfer EVENT_SawTransfer;
struct _EVENT_SawTransferFail {
    int from_;
    int to_;
    int amount;
};
typedef struct _EVENT_SawTransferFail EVENT_SawTransferFail;
struct _EVENT_Clear {};
typedef struct _EVENT_Clear EVENT_Clear;

// input stream for stream type BankEvent
struct _STREAM_BankEvent_in {
    shm_event head;
    union {
        EVENT_SawWithdraw     SawWithdraw;
        EVENT_Clear           Clear;
        EVENT_SawTransfer     SawTransfer;
        EVENT_SawBalance      SawBalance;
        EVENT_SawDeposit      SawDeposit;
        EVENT_SawWithdrawFail SawWithdrawFail;
        EVENT_SawTransferFail SawTransferFail;
    } cases;
};
typedef struct _STREAM_BankEvent_in STREAM_BankEvent_in;

// output stream for stream type BankEvent
struct _STREAM_BankEvent_out {
    shm_event head;
    union {
        EVENT_hole            hole;
        EVENT_SawWithdraw     SawWithdraw;
        EVENT_Clear           Clear;
        EVENT_SawTransfer     SawTransfer;
        EVENT_SawBalance      SawBalance;
        EVENT_SawDeposit      SawDeposit;
        EVENT_SawWithdrawFail SawWithdrawFail;
        EVENT_SawTransferFail SawTransferFail;
    } cases;
};
typedef struct _STREAM_BankEvent_out STREAM_BankEvent_out;

enum BankInputEvent_kinds {
    BANKINPUTEVENT_NUMIN   = 2,
    BANKINPUTEVENT_OTHERIN = 3,
    BANKINPUTEVENT_HOLE    = 1,
};
enum BankOutputEvent_kinds {
    BANKOUTPUTEVENT_BALANCE         = 2,
    BANKOUTPUTEVENT_DEPOSITTO       = 3,
    BANKOUTPUTEVENT_WITHDRAW        = 4,
    BANKOUTPUTEVENT_TRANSFER        = 5,
    BANKOUTPUTEVENT_DEPOSITSUCCESS  = 6,
    BANKOUTPUTEVENT_WITHDRAWSUCCESS = 7,
    BANKOUTPUTEVENT_WITHDRAWFAIL    = 8,
    BANKOUTPUTEVENT_TRANSFERSUCCESS = 9,
    BANKOUTPUTEVENT_SELECTEDACCOUNT = 10,
    BANKOUTPUTEVENT_READINPUT       = 11,
    BANKOUTPUTEVENT_LOGIN           = 12,
    BANKOUTPUTEVENT_NUMOUT          = 13,
    BANKOUTPUTEVENT_HOLE            = 1,
};

int *arbiter_counter;
// monitor buffer
shm_monitor_buffer *monitor_buffer;

bool       is_selection_successful;
dll_node **chosen_streams; // used in rule set for get_first/last_n

// globals code
STREAM_BankEvent_out *arbiter_outevent;

#include "./compiler/cfiles/intmap.h"
intmap balances;
intmap upper_bounds;
intmap lower_bounds;
int    current_account = 0;
int    in_processed    = 0;
int    out_processed   = 0;
int    in_holes        = 0;
int    out_holes       = 0;

bool SHOULD_KEEP_In(shm_stream *s, shm_event *e) {
    return true;
}
bool SHOULD_KEEP_Out(shm_stream *s, shm_event *e) {
    return true;
}

atomic_int count_event_streams = 2;

// declare event streams
shm_stream *EV_SOURCE_In;
shm_stream *EV_SOURCE_Out;

// event sources threads
thrd_t THREAD_In;
thrd_t THREAD_Out;

// declare arbiter thread
thrd_t ARBITER_THREAD;

// Arbiter buffer for event source In
shm_arbiter_buffer *BUFFER_In;

// Arbiter buffer for event source Out
shm_arbiter_buffer *BUFFER_Out;

// buffer groups

int PERF_LAYER_In(shm_arbiter_buffer *buffer) {
    shm_stream                *stream = shm_arbiter_buffer_stream(buffer);
    STREAM_BankInputEvent_in  *inevent;
    STREAM_BankInputEvent_out *outevent;

    // wait for active buffer
    while ((!shm_arbiter_buffer_active(buffer))) {
        sleep_ns(10);
    }
    while (true) {
        inevent = stream_filter_fetch(stream, buffer, &SHOULD_KEEP_In);

        if (inevent == NULL) {
            // no more events
            break;
        }
        outevent = shm_arbiter_buffer_write_ptr(buffer);

        memcpy(outevent, inevent, sizeof(STREAM_BankInputEvent_out));
        shm_arbiter_buffer_write_finish(buffer);
        shm_stream_consume(stream, 1);
    }
    atomic_fetch_add(&count_event_streams, -1);
}
int PERF_LAYER_Out(shm_arbiter_buffer *buffer) {
    shm_stream                 *stream = shm_arbiter_buffer_stream(buffer);
    STREAM_BankOutputEvent_in  *inevent;
    STREAM_BankOutputEvent_out *outevent;

    // wait for active buffer
    while ((!shm_arbiter_buffer_active(buffer))) {
        sleep_ns(10);
    }
    while (true) {
        inevent = stream_filter_fetch(stream, buffer, &SHOULD_KEEP_Out);

        if (inevent == NULL) {
            // no more events
            break;
        }
        outevent = shm_arbiter_buffer_write_ptr(buffer);

        memcpy(outevent, inevent, sizeof(STREAM_BankOutputEvent_out));
        shm_arbiter_buffer_write_finish(buffer);
        shm_stream_consume(stream, 1);
    }
    atomic_fetch_add(&count_event_streams, -1);
}

// variables used to debug arbiter
long unsigned no_consecutive_matches_limit = 1UL << 35;
int           no_matches_count             = 0;

bool are_there_events(shm_arbiter_buffer *b) {
    return shm_arbiter_buffer_size(b) > 0;
}

bool are_buffers_empty() {
    int c = 0;
    c += are_there_events(BUFFER_In);
    c += are_there_events(BUFFER_Out);

    return c == 0;
}

static int __work_done = 0;
/* TODO: make a keywork from this */
void done() {
    __work_done = 1;
}

static bool are_streams_done() {
    assert(count_event_streams >= 0);
    return count_event_streams == 0 && are_buffers_empty() || __work_done;
}

static inline bool check_at_least_n_events(size_t count, size_t n) {
    // count is the result after calling shm_arbiter_buffer_peek
    return count >= n;
}

static bool are_events_in_head(char *e1, size_t i1, char *e2, size_t i2,
                               int count, size_t ev_size, int event_kinds[],
                               int n_events) {
    assert(n_events > 0);
    if (count < n_events) {
        return false;
    }

    int i = 0;
    while (i < i1) {
        shm_event *ev = (shm_event *)(e1);
        if (ev->kind != event_kinds[i]) {
            return false;
        }
        if (--n_events == 0)
            return true;
        i += 1;
        e1 += ev_size;
    }

    i = 0;
    while (i < i2) {
        shm_event *ev = (shm_event *)e2;
        if (ev->kind != event_kinds[i1 + i]) {
            return false;
        }
        if (--n_events == 0)
            return true;
        i += 1;
        e2 += ev_size;
    }

    return true;
}

static inline shm_event *get_event_at_index(char *e1, size_t i1, char *e2,
                                            size_t i2, size_t size_event,
                                            int element_index) {
    if (element_index < i1) {
        return (shm_event *)(e1 + (element_index * size_event));
    } else {
        element_index -= i1;
        return (shm_event *)(e2 + (element_index * size_event));
    }
}

// arbiter outevent
STREAM_BankEvent_out *arbiter_outevent;
int current_rule_set = 0;
int                   RULE_SET_working();
int                   RULE_SET_resetting();

void print_event_name(int ev_src_index, int event_index) {
    if (event_index == -1) {
        printf("None\n");
        return;
    }

    if (event_index == 1) {
        printf("hole\n");
        return;
    }

    if (ev_src_index == 0) {

        if (event_index == 2) {
            printf("numIn\n");
            return;
        }

        if (event_index == 3) {
            printf("otherIn\n");
            return;
        }

        if (event_index == 1) {
            printf("hole\n");
            return;
        }

        printf("No event matched! this should not happen, please report!\n");
        return;
    }

    if (ev_src_index == 1) {

        if (event_index == 2) {
            printf("balance\n");
            return;
        }

        if (event_index == 3) {
            printf("depositTo\n");
            return;
        }

        if (event_index == 4) {
            printf("withdraw\n");
            return;
        }

        if (event_index == 5) {
            printf("transfer\n");
            return;
        }

        if (event_index == 6) {
            printf("depositSuccess\n");
            return;
        }

        if (event_index == 7) {
            printf("withdrawSuccess\n");
            return;
        }

        if (event_index == 8) {
            printf("withdrawFail\n");
            return;
        }

        if (event_index == 9) {
            printf("transferSuccess\n");
            return;
        }

        if (event_index == 10) {
            printf("selectedAccount\n");
            return;
        }

        if (event_index == 11) {
            printf("readInput\n");
            return;
        }

        if (event_index == 12) {
            printf("login\n");
            return;
        }

        if (event_index == 13) {
            printf("numOut\n");
            return;
        }

        if (event_index == 1) {
            printf("hole\n");
            return;
        }

        printf("No event matched! this should not happen, please report!\n");
        return;
    }

    printf("Invalid event source! this should not happen, please report!\n");
}

int get_event_at_head(shm_arbiter_buffer *b) {
    void  *e1;
    size_t i1;
    void  *e2;
    size_t i2;

    int count = shm_arbiter_buffer_peek(b, 0, &e1, &i1, &e2, &i2);
    if (count == 0) {
        return -1;
    }
    shm_event *ev = (shm_event *)(e1);
    return ev->kind;
}

void print_buffers_state() {
    int event_index;
    event_index = get_event_at_head(BUFFER_In);
    printf("In -> ");
    print_event_name(0, event_index);
    event_index = get_event_at_head(BUFFER_Out);
    printf("Out -> ");
    print_event_name(1, event_index);
}

int RULE_SET_working() {
    char  *e1_In;
    size_t i1_In;
    char  *e2_In;
    size_t i2_In;
    int    count_In = shm_arbiter_buffer_peek(BUFFER_In, 2, (void **)&e1_In,
                                              &i1_In, (void **)&e2_In, &i2_In);
    char  *e1_Out;
    size_t i1_Out;
    char  *e2_Out;
    size_t i2_Out;
    int    count_Out = shm_arbiter_buffer_peek(BUFFER_Out, 2, (void **)&e1_Out,
                                               &i1_Out, (void **)&e2_Out, &i2_Out);

    int TEMPARR0[] = {BANKINPUTEVENT_HOLE};

    if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                           sizeof(STREAM_BankInputEvent_out), TEMPARR0, 1)) {

        STREAM_BankInputEvent_out *event_for_n =
            (STREAM_BankInputEvent_out *)get_event_at_index(
                e1_In, i1_In, e2_In, i2_In, sizeof(STREAM_BankInputEvent_out),
                0);
        int n = event_for_n->cases.hole.n;

        if (true) {

            arbiter_outevent =
                (STREAM_BankEvent_out *)shm_monitor_buffer_write_ptr(
                    monitor_buffer);
            arbiter_outevent->head.kind = 8;
            arbiter_outevent->head.id   = (*arbiter_counter)++;

            shm_monitor_buffer_write_finish(monitor_buffer);

            ++in_holes;
	    current_rule_set = 1;

            shm_arbiter_buffer_drop(BUFFER_In, 1);

            return 1;
        }
    }

    int TEMPARR1[] = {BANKOUTPUTEVENT_HOLE};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR1, 1)) {

        STREAM_BankOutputEvent_out *event_for_n =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int n = event_for_n->cases.hole.n;

        if (true) {

            arbiter_outevent =
                (STREAM_BankEvent_out *)shm_monitor_buffer_write_ptr(
                    monitor_buffer);
            arbiter_outevent->head.kind = 8;
            arbiter_outevent->head.id   = (*arbiter_counter)++;

            shm_monitor_buffer_write_finish(monitor_buffer);

            ++out_holes;
	    current_rule_set = 1;

            shm_arbiter_buffer_drop(BUFFER_Out, 1);

            return 1;
        }
    }

    int TEMPARR2[] = {BANKOUTPUTEVENT_BALANCE, BANKOUTPUTEVENT_NUMOUT};
    int TEMPARR3[] = {BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR2, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR3,
                               1)) {

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int acc = event_for_acc->cases.balance.account;

            STREAM_BankOutputEvent_out *event_for_amnt =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            int amnt = event_for_amnt->cases.numOut.num;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            if (true) {

                assert(action == 3);

                arbiter_outevent =
                    (STREAM_BankEvent_out *)shm_monitor_buffer_write_ptr(
                        monitor_buffer);
                arbiter_outevent->head.kind = 2;
                arbiter_outevent->head.id   = (*arbiter_counter)++;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawBalance.account = acc;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawBalance.balance = amnt;

                shm_monitor_buffer_write_finish(monitor_buffer);
                current_account = acc;
                out_processed += 2;
                ++in_processed;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 1);

                return 1;
            }
        }
    }

    int TEMPARR4[] = {BANKOUTPUTEVENT_DEPOSITTO,
                      BANKOUTPUTEVENT_DEPOSITSUCCESS};
    int TEMPARR5[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR4, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR5,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int acc = event_for_acc->cases.depositTo.account;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_amnt =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            int amnt = event_for_amnt->cases.numIn.num;

            if (true) {

                assert(action == 1);

                arbiter_outevent =
                    (STREAM_BankEvent_out *)shm_monitor_buffer_write_ptr(
                        monitor_buffer);
                arbiter_outevent->head.kind = 3;
                arbiter_outevent->head.id   = (*arbiter_counter)++;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawDeposit.account = acc;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawDeposit.amount = amnt;

                shm_monitor_buffer_write_finish(monitor_buffer);
                current_account = acc;
                out_processed += 2;
                in_processed += 2;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 2);

                return 1;
            }
        }
    }

    int TEMPARR6[] = {BANKOUTPUTEVENT_WITHDRAW,
                      BANKOUTPUTEVENT_WITHDRAWSUCCESS};
    int TEMPARR7[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR6, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR7,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int acc = event_for_acc->cases.withdraw.account;

            STREAM_BankInputEvent_out *event_for_act =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int act = event_for_act->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_amnt =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            int amnt = event_for_amnt->cases.numIn.num;

            if (true) {

                assert(act == 2);

                arbiter_outevent =
                    (STREAM_BankEvent_out *)shm_monitor_buffer_write_ptr(
                        monitor_buffer);
                arbiter_outevent->head.kind = 4;
                arbiter_outevent->head.id   = (*arbiter_counter)++;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawWithdraw.account = acc;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawWithdraw.amount = amnt;

                shm_monitor_buffer_write_finish(monitor_buffer);
                out_processed += 2;
                in_processed += 2;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 2);

                return 1;
            }
        }
    }

    int TEMPARR8[] = {BANKOUTPUTEVENT_WITHDRAW, BANKOUTPUTEVENT_WITHDRAWFAIL};
    int TEMPARR9[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR8, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR9,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int acc = event_for_acc->cases.withdraw.account;

            STREAM_BankInputEvent_out *event_for_act =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int act = event_for_act->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_amnt =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            int amnt = event_for_amnt->cases.numIn.num;

            if (true) {

                assert(act == 2);

                arbiter_outevent =
                    (STREAM_BankEvent_out *)shm_monitor_buffer_write_ptr(
                        monitor_buffer);
                arbiter_outevent->head.kind = 5;
                arbiter_outevent->head.id   = (*arbiter_counter)++;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawWithdrawFail.account = acc;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawWithdrawFail.amount = amnt;

                shm_monitor_buffer_write_finish(monitor_buffer);
                out_processed += 2;
                in_processed += 2;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 2);

                return 1;
            }
        }
    }

    int TEMPARR10[] = {BANKOUTPUTEVENT_TRANSFER,
                       BANKOUTPUTEVENT_TRANSFERSUCCESS};
    int TEMPARR11[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR10, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR11,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_from_ =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int from_ = event_for_from_->cases.transfer.fromAccount;

            STREAM_BankOutputEvent_out *event_for_to_ =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int to_ = event_for_to_->cases.transfer.toAccount;

            STREAM_BankInputEvent_out *event_for_acc =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int acc = event_for_acc->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_amnt =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            int amnt = event_for_amnt->cases.numIn.num;

            if (true) {

                arbiter_outevent =
                    (STREAM_BankEvent_out *)shm_monitor_buffer_write_ptr(
                        monitor_buffer);
                arbiter_outevent->head.kind = 6;
                arbiter_outevent->head.id   = (*arbiter_counter)++;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawTransfer.from_ = from_;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawTransfer.to_ = to_;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawTransfer.amount = amnt;

                shm_monitor_buffer_write_finish(monitor_buffer);

                out_processed += 2;
                in_processed += 2;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 2);

                return 1;
            }
        }
    }

    int TEMPARR12[] = {BANKOUTPUTEVENT_TRANSFER,
                       BANKOUTPUTEVENT_SELECTEDACCOUNT};
    int TEMPARR13[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR12, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR13,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_from_ =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int from_ = event_for_from_->cases.transfer.fromAccount;

            STREAM_BankOutputEvent_out *event_for_to_ =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int to_ = event_for_to_->cases.transfer.toAccount;

            STREAM_BankInputEvent_out *event_for_acc =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int acc = event_for_acc->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_amnt =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            int amnt = event_for_amnt->cases.numIn.num;

            if (true) {

                arbiter_outevent =
                    (STREAM_BankEvent_out *)shm_monitor_buffer_write_ptr(
                        monitor_buffer);
                arbiter_outevent->head.kind = 7;
                arbiter_outevent->head.id   = (*arbiter_counter)++;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawTransferFail.from_ = from_;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawTransferFail.to_ = to_;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.SawTransferFail.amount = amnt;

                shm_monitor_buffer_write_finish(monitor_buffer);

                out_processed += 2;
                in_processed += 2;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 2);

                return 1;
            }
        }
    }

    int TEMPARR14[] = {BANKOUTPUTEVENT_SELECTEDACCOUNT};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR14, 1)) {

        STREAM_BankOutputEvent_out *event_for_acc =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int acc = event_for_acc->cases.selectedAccount.account;

        if (true) {

            current_account = acc;
            ++out_processed;

            shm_arbiter_buffer_drop(BUFFER_Out, 1);

            return 1;
        }
    }

    int TEMPARR15[] = {BANKOUTPUTEVENT_LOGIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR15, 1)) {
        if (check_at_least_n_events(count_In, 2)) {

            if (true) {

                shm_arbiter_buffer_drop(BUFFER_In, 2);

                ++out_processed;
                in_processed += 2;

                shm_arbiter_buffer_drop(BUFFER_Out, 1);

                return 1;
            }
        }
    }

    int TEMPARR16[] = {BANKOUTPUTEVENT_READINPUT};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR16, 1)) {
        if (check_at_least_n_events(count_In, 1)) {

            if (true) {

                shm_arbiter_buffer_drop(BUFFER_In, 1);

                ++out_processed;
                ++in_processed;

                shm_arbiter_buffer_drop(BUFFER_Out, 1);

                return 1;
            }
        }
    }

    int TEMPARR17[] = {BANKOUTPUTEVENT_NUMOUT};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR17, 1)) {

        STREAM_BankOutputEvent_out *event_for_n =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int n = event_for_n->cases.numOut.num;

        if (true) {

            ++out_processed;

            shm_arbiter_buffer_drop(BUFFER_Out, 1);

            return 1;
        }
    }
    return 0;
}
int RULE_SET_resetting() {
    char  *e1_Out;
    size_t i1_Out;
    char  *e2_Out;
    size_t i2_Out;
    int    count_Out = shm_arbiter_buffer_peek(BUFFER_Out, 2, (void **)&e1_Out,
                                               &i1_Out, (void **)&e2_Out, &i2_Out);
    char  *e1_In;
    size_t i1_In;
    char  *e2_In;
    size_t i2_In;
    int    count_In = shm_arbiter_buffer_peek(BUFFER_In, 3, (void **)&e1_In,
                                              &i1_In, (void **)&e2_In, &i2_In);

    int TEMPARR18[] = {BANKOUTPUTEVENT_LOGIN, BANKOUTPUTEVENT_READINPUT};
    int TEMPARR19[] = {BANKINPUTEVENT_OTHERIN, BANKINPUTEVENT_OTHERIN,
                       BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR18, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR19,
                               3)) {

            STREAM_BankInputEvent_out *event_for_n =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 2);
            int n = event_for_n->cases.numIn.num;

            if (n == 1 || n == 0) {

		current_rule_set = 0;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 3);

                return 1;
            }
        }
    }
    return 0;
}
int arbiter() {

    while (!are_streams_done()) {
        int rule_sets_match_count = 0;
	switch(current_rule_set) {
	case 0:
            rule_sets_match_count += RULE_SET_working();
	    break;
	case 1:
            rule_sets_match_count += RULE_SET_resetting();
	    break;
        }

        if (rule_sets_match_count == 0) {
            // increment counter of no consecutive matches
            no_matches_count++;
        } else {
            // if there is a match reinit counter
            no_matches_count = 0;
        }

        if (no_matches_count == no_consecutive_matches_limit) {
            printf("******** NO RULES MATCHED FOR %d ITERATIONS, exiting "
                   "program... **************\n",
                   no_consecutive_matches_limit);
            print_buffers_state();
            // cleanup code
            printf("\nin_processed: %i, out_processed: %i\n", in_processed,
                   out_processed);
            printf("\nin_holes: %i, out_holes: %i\n", in_holes, out_holes);
            destroy_intmap(&balances);
            destroy_intmap(&upper_bounds);
            destroy_intmap(&lower_bounds);

            abort();
        }
    }
    shm_monitor_set_finished(monitor_buffer);
}

int main(int argc, char **argv) {
    chosen_streams = (dll_node *)malloc(2); // the maximum size this can have is
                                            // the total number of event sources
    arbiter_counter  = malloc(sizeof(int));
    *arbiter_counter = 10;
    init_intmap(&balances);
    init_intmap(&upper_bounds);
    init_intmap(&lower_bounds);

    // connect to event source In
    EV_SOURCE_In = shm_stream_create_from_argv("In", argc, argv);
    BUFFER_In    = shm_arbiter_buffer_create(
           EV_SOURCE_In, sizeof(STREAM_BankInputEvent_out), 128);

    // register events in In
    if (shm_stream_register_event(EV_SOURCE_In, "numIn", BANKINPUTEVENT_NUMIN) <
        0) {
        fprintf(
            stderr,
            "Failed registering event numIn for stream In : BankInputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_In);
        abort();
    }
    if (shm_stream_register_event(EV_SOURCE_In, "otherIn",
                                  BANKINPUTEVENT_OTHERIN) < 0) {
        fprintf(stderr, "Failed registering event otherIn for stream In : "
                        "BankInputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_In);
        abort();
    }
    // connect to event source Out
    EV_SOURCE_Out = shm_stream_create_from_argv("Out", argc, argv);
    BUFFER_Out    = shm_arbiter_buffer_create(
           EV_SOURCE_Out, sizeof(STREAM_BankOutputEvent_out), 128);

    // register events in Out
    if (shm_stream_register_event(EV_SOURCE_Out, "balance",
                                  BANKOUTPUTEVENT_BALANCE) < 0) {
        fprintf(stderr, "Failed registering event balance for stream Out : "
                        "BankOutputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_Out);
        abort();
    }
    if (shm_stream_register_event(EV_SOURCE_Out, "depositTo",
                                  BANKOUTPUTEVENT_DEPOSITTO) < 0) {
        fprintf(stderr, "Failed registering event depositTo for stream Out : "
                        "BankOutputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_Out);
        abort();
    }
    if (shm_stream_register_event(EV_SOURCE_Out, "withdraw",
                                  BANKOUTPUTEVENT_WITHDRAW) < 0) {
        fprintf(stderr, "Failed registering event withdraw for stream Out : "
                        "BankOutputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_Out);
        abort();
    }
    if (shm_stream_register_event(EV_SOURCE_Out, "transfer",
                                  BANKOUTPUTEVENT_TRANSFER) < 0) {
        fprintf(stderr, "Failed registering event transfer for stream Out : "
                        "BankOutputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_Out);
        abort();
    }
    if (shm_stream_register_event(EV_SOURCE_Out, "depositSuccess",
                                  BANKOUTPUTEVENT_DEPOSITSUCCESS) < 0) {
        fprintf(stderr, "Failed registering event depositSuccess for stream "
                        "Out : BankOutputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_Out);
        abort();
    }
    if (shm_stream_register_event(EV_SOURCE_Out, "withdrawSuccess",
                                  BANKOUTPUTEVENT_WITHDRAWSUCCESS) < 0) {
        fprintf(stderr, "Failed registering event withdrawSuccess for stream "
                        "Out : BankOutputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_Out);
        abort();
    }
    if (shm_stream_register_event(EV_SOURCE_Out, "withdrawFail",
                                  BANKOUTPUTEVENT_WITHDRAWFAIL) < 0) {
        fprintf(stderr, "Failed registering event withdrawFail for stream Out "
                        ": BankOutputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_Out);
        abort();
    }
    if (shm_stream_register_event(EV_SOURCE_Out, "transferSuccess",
                                  BANKOUTPUTEVENT_TRANSFERSUCCESS) < 0) {
        fprintf(stderr, "Failed registering event transferSuccess for stream "
                        "Out : BankOutputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_Out);
        abort();
    }
    if (shm_stream_register_event(EV_SOURCE_Out, "selectedAccount",
                                  BANKOUTPUTEVENT_SELECTEDACCOUNT) < 0) {
        fprintf(stderr, "Failed registering event selectedAccount for stream "
                        "Out : BankOutputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_Out);
        abort();
    }
    if (shm_stream_register_event(EV_SOURCE_Out, "readInput",
                                  BANKOUTPUTEVENT_READINPUT) < 0) {
        fprintf(stderr, "Failed registering event readInput for stream Out : "
                        "BankOutputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_Out);
        abort();
    }
    if (shm_stream_register_event(EV_SOURCE_Out, "login",
                                  BANKOUTPUTEVENT_LOGIN) < 0) {
        fprintf(stderr, "Failed registering event login for stream Out : "
                        "BankOutputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_Out);
        abort();
    }
    if (shm_stream_register_event(EV_SOURCE_Out, "numOut",
                                  BANKOUTPUTEVENT_NUMOUT) < 0) {
        fprintf(stderr, "Failed registering event numOut for stream Out : "
                        "BankOutputEvent\n");
        fprintf(stderr, "Available events:\n");
        shm_stream_dump_events(EV_SOURCE_Out);
        abort();
    }

    // activate buffers
    shm_arbiter_buffer_set_active(BUFFER_In, true);
    shm_arbiter_buffer_set_active(BUFFER_Out, true);

    monitor_buffer = shm_monitor_buffer_create(sizeof(STREAM_BankEvent_out), 8);

    // init buffer groups

    // create source-events threads
    thrd_create(&THREAD_In, PERF_LAYER_In, BUFFER_In);
    thrd_create(&THREAD_Out, PERF_LAYER_Out, BUFFER_Out);

    // create arbiter thread
    thrd_create(&ARBITER_THREAD, arbiter, 0);

    // monitor
    STREAM_BankEvent_out *received_event;
    while (true) {
        received_event = fetch_arbiter_stream(monitor_buffer);
        if (received_event == NULL) {
            break;
        }

        if (received_event->head.kind == 8) {

            if (true) {
                intmap_clear(&balances);
                intmap_clear(&upper_bounds);
                intmap_clear(&lower_bounds);
            }
        }

        if (received_event->head.kind == 2) {
            int account = received_event->cases.SawBalance.account;
            int balance = received_event->cases.SawBalance.balance;

            if (true) {
                int old_value = 0;
                if (intmap_get(&balances, account, &old_value)) {
                    if (old_value != balance) {
                        printf("Mismatched balance on account #%i: Expected "
                               "%i, but got %i\n",
                               account, old_value, balance);
                        intmap_insert(&balances, account, balance);
                    }
                } else {
                    if (intmap_get(&upper_bounds, account, &old_value)) {
                        if (balance > old_value) {
                            printf("Mismatched balance on account #%i: "
                                   "Expected at most %i, but got %i\n",
                                   account, old_value, balance);
                        }
                        intmap_remove(&upper_bounds, account);
                    }
                    if (intmap_get(&lower_bounds, account, &old_value)) {
                        if (balance < old_value) {
                            printf("Mismatched balance on account #%i: "
                                   "Expected at least %i, but got %i\n",
                                   account, old_value, balance);
                        }
                        intmap_remove(&lower_bounds, account);
                    }
                    intmap_insert(&balances, account, balance);
                }
            }
        }

        if (received_event->head.kind == 3) {
            int account = received_event->cases.SawDeposit.account;
            int amount  = received_event->cases.SawDeposit.amount;

            if (true) {
                int old_value = 0;
                if (intmap_get(&balances, account, &old_value)) {
                    intmap_insert(&balances, account, old_value + amount);
                } else {
                    if (intmap_get(&upper_bounds, account, &old_value)) {
                        intmap_insert(&upper_bounds, account,
                                      old_value + amount);
                    }
                    if (intmap_get(&lower_bounds, account, &old_value)) {
                        intmap_insert(&lower_bounds, account,
                                      old_value + amount);
                    } else {
                        intmap_insert(&lower_bounds, account, amount);
                    }
                }
            }
        }

        if (received_event->head.kind == 4) {
            int account = received_event->cases.SawWithdraw.account;
            int amount  = received_event->cases.SawWithdraw.amount;

            if (true) {
                int old_value = 0;
                if (intmap_get(&balances, account, &old_value)) {
                    if (old_value >= amount) {
                        intmap_insert(&balances, account, old_value - amount);
                    } else {
                        printf("Withdrawal of %i from account #%i should have "
                               "failed based on balance %i\n",
                               amount, account, old_value);
                        intmap_remove(&balances, account);
                    }
                } else {
                    if (intmap_get(&upper_bounds, account, &old_value)) {
                        if (old_value >= amount) {
                            intmap_insert(&upper_bounds, account,
                                          old_value - amount);
                        } else {
                            printf(
                                "Withdrawal of %i from account #%i should have "
                                "failed based on upper balance bound %i\n",
                                amount, account, old_value);
                            intmap_remove(&upper_bounds, account);
                        }
                    }
                    if (intmap_get(&lower_bounds, account, &old_value)) {
                        if (old_value >= amount) {
                            intmap_insert(&lower_bounds, account,
                                          old_value - amount);
                        } else {
                            intmap_insert(&lower_bounds, account, 0);
                        }
                    }
                }
            }
        }

        if (received_event->head.kind == 5) {
            int account = received_event->cases.SawWithdrawFail.account;
            int amount  = received_event->cases.SawWithdrawFail.amount;

            if (true) {
                int old_value = 0;
                if (intmap_get(&balances, account, &old_value)) {
                    if (old_value >= amount) {
                        printf("Withdrawal of %i from account #%i should have "
                               "succeeded based on balance %i\n",
                               amount, account, old_value);
                        intmap_remove(&balances, account);
                    }
                } else {
                    if (intmap_get(&upper_bounds, account, &old_value)) {
                        if (old_value < amount) {
                            intmap_insert(&upper_bounds, account, amount);
                        }
                    } else {
                        intmap_insert(&upper_bounds, account, amount);
                    }
                    if (intmap_get(&lower_bounds, account, &old_value)) {
                        if (old_value >= amount) {
                            printf(
                                "Withdrawal of %i from account #%i should have "
                                "succeeded based on lower balance bound %i\n",
                                amount, account, old_value);
                            intmap_remove(&lower_bounds, account);
                        }
                    }
                }
            }
        }

        if (received_event->head.kind == 6) {
            int from_  = received_event->cases.SawTransfer.from_;
            int to_    = received_event->cases.SawTransfer.to_;
            int amount = received_event->cases.SawTransfer.amount;

            if (true) {
                int old_value = 0;
                if (intmap_get(&balances, to_, &old_value)) {
                    intmap_insert(&balances, to_, old_value + amount);
                } else {
                    if (intmap_get(&upper_bounds, to_, &old_value)) {
                        intmap_insert(&upper_bounds, to_, old_value + amount);
                    }
                    if (intmap_get(&lower_bounds, to_, &old_value)) {
                        intmap_insert(&lower_bounds, to_, old_value + amount);
                    } else {
                        intmap_insert(&lower_bounds, to_, amount);
                    }
                }

                if (intmap_get(&balances, from_, &old_value)) {
                    if (old_value >= amount) {
                        intmap_insert(&balances, from_, old_value - amount);
                    } else {
                        printf("Transfer of %i from account #%i should have "
                               "failed based on balance %i\n",
                               amount, from_, old_value);
                        intmap_remove(&balances, from_);
                    }
                } else {
                    if (intmap_get(&upper_bounds, from_, &old_value)) {
                        if (old_value >= amount) {
                            intmap_insert(&upper_bounds, from_,
                                          old_value - amount);
                        } else {
                            printf(
                                "Transfer of %i from account #%i should have "
                                "failed based on upper balance bound %i\n",
                                amount, from_, old_value);
                            intmap_remove(&upper_bounds, from_);
                        }
                    }
                    if (intmap_get(&lower_bounds, from_, &old_value)) {
                        if (old_value >= amount) {
                            intmap_insert(&lower_bounds, from_,
                                          old_value - amount);
                        } else {
                            intmap_insert(&lower_bounds, from_, 0);
                        }
                    }
                }
            }
        }

        if (received_event->head.kind == 7) {
            int from_  = received_event->cases.SawTransferFail.from_;
            int to_    = received_event->cases.SawTransferFail.to_;
            int amount = received_event->cases.SawTransferFail.amount;

            if (true) {
                int old_value = 0;
                if (intmap_get(&balances, from_, &old_value)) {
                    if (old_value >= amount) {
                        printf("Transfer of %i from account #%i should have "
                               "succeeded based on balance %i\n",
                               amount, from_, old_value);
                        intmap_remove(&balances, from_);
                    }
                } else {
                    if (intmap_get(&upper_bounds, from_, &old_value)) {
                        if (old_value < amount) {
                            intmap_insert(&upper_bounds, from_, amount);
                        }
                    } else {
                        intmap_insert(&upper_bounds, from_, amount);
                    }
                    if (intmap_get(&lower_bounds, from_, &old_value)) {
                        if (old_value >= amount) {
                            printf(
                                "Transfer of %i from account #%i should have "
                                "succeeded based on lower balance bound %i\n",
                                amount, from_, old_value);
                            intmap_remove(&lower_bounds, from_);
                        }
                    }
                }
            }
        }

        shm_monitor_buffer_consume(monitor_buffer, 1);
    }

    shm_stream_destroy(EV_SOURCE_In);
    shm_stream_destroy(EV_SOURCE_Out);
    shm_arbiter_buffer_free(BUFFER_In);
    shm_arbiter_buffer_free(BUFFER_Out);
    free(arbiter_counter);
    free(monitor_buffer);
    free(chosen_streams);

    printf("\nin_processed: %i, out_processed: %i\n", in_processed,
           out_processed);
    printf("\nin_holes: %i, out_holes: %i\n", in_holes, out_holes);
    destroy_intmap(&balances);
    destroy_intmap(&upper_bounds);
    destroy_intmap(&lower_bounds);
}
