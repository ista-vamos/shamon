

#include "../../../compiler/cfiles/compiler_utils.h"
#include "mmlib.h"
#include "monitor.h"
#include "shamon.h"
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <threads.h>

struct _EVENT_hole {
    uint64_t n;
};
typedef struct _EVENT_hole EVENT_hole;
// event declarations for stream type BankOutputEvent
struct _EVENT_balance {
    uint64_t timestamp;
    int      account;
};
typedef struct _EVENT_balance EVENT_balance;
struct _EVENT_depositTo {
    uint64_t timestamp;
    int      account;
};
typedef struct _EVENT_depositTo EVENT_depositTo;
struct _EVENT_withdraw {
    uint64_t timestamp;
    int      account;
};
typedef struct _EVENT_withdraw EVENT_withdraw;
struct _EVENT_transfer {
    uint64_t timestamp;
    int      fromAccount;
    int      toAccount;
};
typedef struct _EVENT_transfer EVENT_transfer;
struct _EVENT_depositSuccess {
    uint64_t timestamp;
};
typedef struct _EVENT_depositSuccess EVENT_depositSuccess;
struct _EVENT_depositFail {
    uint64_t timestamp;
};
typedef struct _EVENT_depositFail EVENT_depositFail;
struct _EVENT_withdrawSuccess {
    uint64_t timestamp;
};
typedef struct _EVENT_withdrawSuccess EVENT_withdrawSuccess;
struct _EVENT_withdrawFail {
    uint64_t timestamp;
};
typedef struct _EVENT_withdrawFail EVENT_withdrawFail;
struct _EVENT_transferSuccess {
    uint64_t timestamp;
};
typedef struct _EVENT_transferSuccess EVENT_transferSuccess;
struct _EVENT_selectedAccount {
    uint64_t timestamp;
    int      account;
};
typedef struct _EVENT_selectedAccount EVENT_selectedAccount;
struct _EVENT_logout {
    uint64_t timestamp;
};
typedef struct _EVENT_logout EVENT_logout;
struct _EVENT_numOut {
    uint64_t timestamp;
    int      num;
};
typedef struct _EVENT_numOut EVENT_numOut;

// input stream for stream type BankOutputEvent
struct _STREAM_BankOutputEvent_in {
    shm_event head;
    union {
        EVENT_balance         balance;
        EVENT_logout          logout;
        EVENT_withdrawFail    withdrawFail;
        EVENT_transferSuccess transferSuccess;
        EVENT_transfer        transfer;
        EVENT_numOut          numOut;
        EVENT_withdraw        withdraw;
        EVENT_depositTo       depositTo;
        EVENT_depositSuccess  depositSuccess;
        EVENT_depositFail     depositFail;
        EVENT_withdrawSuccess withdrawSuccess;
        EVENT_selectedAccount selectedAccount;
    } cases;
};
typedef struct _STREAM_BankOutputEvent_in STREAM_BankOutputEvent_in;

// output stream for stream type BankOutputEvent
struct _STREAM_BankOutputEvent_out {
    shm_event head;
    union {
        EVENT_hole            hole;
        EVENT_balance         balance;
        EVENT_logout          logout;
        EVENT_withdrawFail    withdrawFail;
        EVENT_transferSuccess transferSuccess;
        EVENT_transfer        transfer;
        EVENT_numOut          numOut;
        EVENT_withdraw        withdraw;
        EVENT_depositTo       depositTo;
        EVENT_depositSuccess  depositSuccess;
        EVENT_depositFail     depositFail;
        EVENT_withdrawSuccess withdrawSuccess;
        EVENT_selectedAccount selectedAccount;
    } cases;
};
typedef struct _STREAM_BankOutputEvent_out STREAM_BankOutputEvent_out;
// event declarations for stream type BankInputEvent
struct _EVENT_numIn {
    uint64_t timestamp;
    int      num;
};
typedef struct _EVENT_numIn EVENT_numIn;
struct _EVENT_otherIn {
    uint64_t timestamp;
};
typedef struct _EVENT_otherIn EVENT_otherIn;

// input stream for stream type BankInputEvent
struct _STREAM_BankInputEvent_in {
    shm_event head;
    union {
        EVENT_numIn   numIn;
        EVENT_otherIn otherIn;
    } cases;
};
typedef struct _STREAM_BankInputEvent_in STREAM_BankInputEvent_in;

// output stream for stream type BankInputEvent
struct _STREAM_BankInputEvent_out {
    shm_event head;
    union {
        EVENT_hole    hole;
        EVENT_numIn   numIn;
        EVENT_otherIn otherIn;
    } cases;
};
typedef struct _STREAM_BankInputEvent_out STREAM_BankInputEvent_out;
// event declarations for stream type BankEvent
struct _EVENT_monbalance {
    int account;
    int balance;
};
typedef struct _EVENT_monbalance EVENT_monbalance;
struct _EVENT_deposit {
    int account;
    int amount;
};
typedef struct _EVENT_deposit EVENT_deposit;
struct _EVENT_monwithdraw {
    int  account;
    int  amount;
    bool success;
};
typedef struct _EVENT_monwithdraw EVENT_monwithdraw;
struct _EVENT_montransfer {
    int  from_;
    int  to_;
    int  amount;
    bool success;
};
typedef struct _EVENT_montransfer EVENT_montransfer;
struct _EVENT_reset {};
typedef struct _EVENT_reset EVENT_reset;

// input stream for stream type BankEvent
struct _STREAM_BankEvent_in {
    shm_event head;
    union {
        EVENT_monbalance  monbalance;
        EVENT_deposit     deposit;
        EVENT_monwithdraw monwithdraw;
        EVENT_reset       reset;
        EVENT_montransfer montransfer;
    } cases;
};
typedef struct _STREAM_BankEvent_in STREAM_BankEvent_in;

// output stream for stream type BankEvent
struct _STREAM_BankEvent_out {
    shm_event head;
    union {
        EVENT_hole        hole;
        EVENT_monbalance  monbalance;
        EVENT_deposit     deposit;
        EVENT_monwithdraw monwithdraw;
        EVENT_reset       reset;
        EVENT_montransfer montransfer;
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
    BANKOUTPUTEVENT_DEPOSITFAIL     = 7,
    BANKOUTPUTEVENT_WITHDRAWSUCCESS = 8,
    BANKOUTPUTEVENT_WITHDRAWFAIL    = 9,
    BANKOUTPUTEVENT_TRANSFERSUCCESS = 10,
    BANKOUTPUTEVENT_SELECTEDACCOUNT = 11,
    BANKOUTPUTEVENT_LOGOUT          = 12,
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
int    in_processed  = 0;
int    out_processed = 0;
int    in_holes      = 0;
int    out_holes     = 0;
int    errors        = 0;
size_t timestamp     = 0;
FILE  *tessla_in;

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
thrd_t    ARBITER_THREAD;
const int SWITCH_TO_RULE_SET_aligning = 0;
const int SWITCH_TO_RULE_SET_align_in = 1;
const int SWITCH_TO_RULE_SET_working  = 2;

static size_t RULE_SET_aligning_nomatch_cnt = 0;
static size_t RULE_SET_align_in_nomatch_cnt = 0;
static size_t RULE_SET_working_nomatch_cnt  = 0;

int current_rule_set = SWITCH_TO_RULE_SET_aligning;

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

static inline bool are_streams_done() {
    assert(count_event_streams >= 0);
    return count_event_streams == 0 && are_buffers_empty() || __work_done;
}

static inline bool is_stream_done(shm_stream *s) {
    return !shm_stream_is_ready(s);
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

#define vamos_check(cond)                                                      \
    do {                                                                       \
        if (!cond) {                                                           \
            fprintf(stderr, "[31m%s:%s:%d: check '" #cond "' failed![0m\n",  \
                    __FILE__, __func__, __LINE__);                             \
            print_buffers_state();                                             \
        }                                                                      \
    } while (0)
#define vamos_assert(cond)                                                     \
    do {                                                                       \
        if (!cond) {                                                           \
            fprintf(stderr, "[31m%s:%s:%d: assert '" #cond "' failed![0m\n", \
                    __FILE__, __func__, __LINE__);                             \
            print_buffers_state();                                             \
            __work_done = 1;                                                   \
        }                                                                      \
    } while (0)
#define vamos_hard_assert(cond)                                                \
    do {                                                                       \
        if (!cond) {                                                           \
            fprintf(stderr, "[31m%s:%s:%d: assert '" #cond "' failed![0m\n", \
                    __FILE__, __func__, __LINE__);                             \
            print_buffers_state();                                             \
            __work_done = 1;                                                   \
            abort();                                                           \
        }                                                                      \
    } while (0)

/*
static inline dump_event_data(shm_event *ev, size_t ev_size) {
    unsigned char *data = ev;
    fprintf(stderr, "[");
    for (unsigned i = sizeof(*ev); i < ev_size; ++i) {
        fprintf(stderr, "0x%x ", data[i]);
    }
    fprintf(stderr, "]");
}
*/

const char *get_event_name(int ev_src_index, int event_index) {
    if (event_index == -1) {
        return "<none>";
    }

    if (event_index == 1) {
        return "hole";
    }

    if (ev_src_index == 0) {

        if (event_index == 2) {
            return "numIn";
        }

        if (event_index == 3) {
            return "otherIn";
        }

        if (event_index == 1) {
            return "hole";
        }

        fprintf(stderr,
                "No event matched! this should not happen, please report!\n");
        return "";
    }

    if (ev_src_index == 1) {

        if (event_index == 2) {
            return "balance";
        }

        if (event_index == 3) {
            return "depositTo";
        }

        if (event_index == 4) {
            return "withdraw";
        }

        if (event_index == 5) {
            return "transfer";
        }

        if (event_index == 6) {
            return "depositSuccess";
        }

        if (event_index == 7) {
            return "depositFail";
        }

        if (event_index == 8) {
            return "withdrawSuccess";
        }

        if (event_index == 9) {
            return "withdrawFail";
        }

        if (event_index == 10) {
            return "transferSuccess";
        }

        if (event_index == 11) {
            return "selectedAccount";
        }

        if (event_index == 12) {
            return "logout";
        }

        if (event_index == 13) {
            return "numOut";
        }

        if (event_index == 1) {
            return "hole";
        }

        fprintf(stderr,
                "No event matched! this should not happen, please report!\n");
        return "";
    }

    printf("Invalid event source! this should not happen, please report!\n");
}

/* src_idx = -1 if unknown */
static void print_buffer_prefix(shm_arbiter_buffer *b, int src_idx,
                                size_t n_events, int cnt, char *e1, size_t i1,
                                char *e2, size_t i2) {
    if (cnt == 0) {
        fprintf(stderr, " empty\n");
        return;
    }
    const size_t ev_size = shm_arbiter_buffer_elem_size(b);
    int          n       = 0;
    int          i       = 0;
    while (i < i1) {
        shm_event *ev = (shm_event *)(e1);
        fprintf(stderr, "  %d: {id: %5lu, kind: %3lu", ++n, shm_event_id(ev),
                shm_event_kind(ev));
        if (src_idx != -1)
            fprintf(stderr, " -> %-12s",
                    get_event_name(src_idx, shm_event_kind(ev)));
        /*dump_event_data(ev, ev_size);*/
        fprintf(stderr, "}\n");
        if (--n_events == 0)
            return;
        i += 1;
        e1 += ev_size;
    }

    i = 0;
    while (i < i2) {
        shm_event *ev = (shm_event *)e2;
        fprintf(stderr, "  %d: {id: %5lu, kind: %3lu", ++n, shm_event_id(ev),
                shm_event_kind(ev));
        if (src_idx != -1)
            fprintf(stderr, " -> %-12s",
                    get_event_name(src_idx, shm_event_kind(ev)));
        /*dump_event_data(ev, ev_size);*/
        fprintf(stderr, "}\n");

        if (--n_events == 0)
            return;
        i += 1;
        e2 += ev_size;
    }
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
int                   RULE_SET_aligning();
int                   RULE_SET_align_in();
int                   RULE_SET_working();

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
            printf("depositFail\n");
            return;
        }

        if (event_index == 8) {
            printf("withdrawSuccess\n");
            return;
        }

        if (event_index == 9) {
            printf("withdrawFail\n");
            return;
        }

        if (event_index == 10) {
            printf("transferSuccess\n");
            return;
        }

        if (event_index == 11) {
            printf("selectedAccount\n");
            return;
        }

        if (event_index == 12) {
            printf("logout\n");
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
    int    event_index;
    int    count;
    char **e1, **e2;
    size_t i1, i2;

    fprintf(stderr, "Prefix of 'In':\n");
    count = shm_arbiter_buffer_peek(BUFFER_In, 10, (void **)&e1, &i1,
                                    (void **)&e2, &i2);
    print_buffer_prefix(BUFFER_In, 0, i1 + i2, count, e1, i1, e2, i2);
    fprintf(stderr, "Prefix of 'Out':\n");
    count = shm_arbiter_buffer_peek(BUFFER_Out, 10, (void **)&e1, &i1,
                                    (void **)&e2, &i2);
    print_buffer_prefix(BUFFER_Out, 1, i1 + i2, count, e1, i1, e2, i2);
}

int RULE_SET_aligning() {
    char  *e1_Out;
    size_t i1_Out;
    char  *e2_Out;
    size_t i2_Out;
    int    count_Out = shm_arbiter_buffer_peek(BUFFER_Out, 1, (void **)&e1_Out,
                                               &i1_Out, (void **)&e2_Out, &i2_Out);

    int TEMPARR0[] = {BANKOUTPUTEVENT_HOLE};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR0, 1)) {

        STREAM_BankOutputEvent_out *event_for_n =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int n = event_for_n->cases.hole.n;

        if (true) {

            arbiter_outevent =
                (STREAM_BankEvent_out *)shm_monitor_buffer_write_ptr(
                    monitor_buffer);
            arbiter_outevent->head.kind = 6;
            arbiter_outevent->head.id   = (*arbiter_counter)++;

            shm_monitor_buffer_write_finish(monitor_buffer);

            ++out_holes;
            ++out_processed;

            shm_arbiter_buffer_drop(BUFFER_Out, 1);

            return 1;
        }
    }

    int TEMPARR1[] = {BANKOUTPUTEVENT_BALANCE};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR1, 1)) {

        STREAM_BankOutputEvent_out *event_for_t =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t = event_for_t->cases.balance.timestamp;

        STREAM_BankOutputEvent_out *event_for_a =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int a = event_for_a->cases.balance.account;

        if (true) {

            current_rule_set = SWITCH_TO_RULE_SET_align_in;

            return 1;
        }
    }

    int TEMPARR2[] = {BANKOUTPUTEVENT_DEPOSITTO};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR2, 1)) {

        STREAM_BankOutputEvent_out *event_for_t =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t = event_for_t->cases.depositTo.timestamp;

        STREAM_BankOutputEvent_out *event_for_a =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int a = event_for_a->cases.depositTo.account;

        if (true) {

            current_rule_set = SWITCH_TO_RULE_SET_align_in;

            return 1;
        }
    }

    int TEMPARR3[] = {BANKOUTPUTEVENT_WITHDRAW};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR3, 1)) {

        STREAM_BankOutputEvent_out *event_for_t =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t = event_for_t->cases.withdraw.timestamp;

        STREAM_BankOutputEvent_out *event_for_a =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int a = event_for_a->cases.withdraw.account;

        if (true) {

            current_rule_set = SWITCH_TO_RULE_SET_align_in;

            return 1;
        }
    }

    int TEMPARR4[] = {BANKOUTPUTEVENT_TRANSFER};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR4, 1)) {

        STREAM_BankOutputEvent_out *event_for_t =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t = event_for_t->cases.transfer.timestamp;

        STREAM_BankOutputEvent_out *event_for_s =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int s = event_for_s->cases.transfer.fromAccount;

        STREAM_BankOutputEvent_out *event_for_r =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int r = event_for_r->cases.transfer.toAccount;

        if (true) {

            current_rule_set = SWITCH_TO_RULE_SET_align_in;

            return 1;
        }
    }

    if (check_at_least_n_events(count_Out, 1)) {

        if (true) {

            ++out_processed;

            shm_arbiter_buffer_drop(BUFFER_Out, 1);

            return 1;
        }
    }

    if (count_Out == 0 && is_stream_done(EV_SOURCE_Out)) {

        if (true) {

            current_rule_set = SWITCH_TO_RULE_SET_align_in;

            return 1;
        }
    }

    _Bool ok = 1;
    if (count_Out >= 1) {
        ok = 0;
    }
    if (ok == 0) {
        fprintf(stderr, "Prefix of 'Out':\n");
        count_Out = shm_arbiter_buffer_peek(BUFFER_Out, 5, &e1_Out, &i1_Out,
                                            &e2_Out, &i2_Out);
        print_buffer_prefix(BUFFER_Out, 1, i1_Out + i2_Out, count_Out, e1_Out,
                            i1_Out, e2_Out, i2_Out);
        fprintf(stderr,
                "No rule in rule set '{rule_set_name}' matched even though "
                "there was enough events, CYCLING WITH NO PROGRESS!\n");
        abort();
    }
    if (++RULE_SET_aligning_nomatch_cnt > 8000000) {
        RULE_SET_aligning_nomatch_cnt = 0;
        fprintf(stderr, "\033[31mRule set 'aligning' cycles long time without "
                        "progress\033[0m\n");
        fprintf(stderr, "Prefix of 'Out':\n");
        count_Out = shm_arbiter_buffer_peek(BUFFER_Out, 5, &e1_Out, &i1_Out,
                                            &e2_Out, &i2_Out);
        print_buffer_prefix(BUFFER_Out, 1, i1_Out + i2_Out, count_Out, e1_Out,
                            i1_Out, e2_Out, i2_Out);
        fprintf(stderr, "Seems all rules are waiting for some events that are "
                        "not coming\n");
    }
    return 0;
}
int RULE_SET_align_in() {
    char  *e1_Out;
    size_t i1_Out;
    char  *e2_Out;
    size_t i2_Out;
    int    count_Out = shm_arbiter_buffer_peek(BUFFER_Out, 1, (void **)&e1_Out,
                                               &i1_Out, (void **)&e2_Out, &i2_Out);
    char  *e1_In;
    size_t i1_In;
    char  *e2_In;
    size_t i2_In;
    int    count_In = shm_arbiter_buffer_peek(BUFFER_In, 1, (void **)&e1_In,
                                              &i1_In, (void **)&e2_In, &i2_In);

    int TEMPARR5[] = {BANKOUTPUTEVENT_BALANCE};
    int TEMPARR6[] = {BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR5, 1)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR6,
                               1)) {

            STREAM_BankOutputEvent_out *event_for_tout =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t tout = event_for_tout->cases.balance.timestamp;

            STREAM_BankOutputEvent_out *event_for_account =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int account = event_for_account->cases.balance.account;

            STREAM_BankInputEvent_out *event_for_tin =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t tin = event_for_tin->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            if (true) {

                if (tout == tin + 1) {
                    assert(action == 3);

                    current_rule_set = SWITCH_TO_RULE_SET_working;
                } else if (tout > tin) {
                    ++in_processed;

                    shm_arbiter_buffer_drop(BUFFER_In, 1);
                } else {
                    /* the event is incomplete, there must have been some hole,
                       try once more */
                    assert(in_holes > 0);
                    ++out_processed;

                    shm_arbiter_buffer_drop(BUFFER_Out, 1);
                    current_rule_set = SWITCH_TO_RULE_SET_aligning;
                }

                return 1;
            }
        }
    }

    int TEMPARR7[] = {BANKOUTPUTEVENT_DEPOSITTO};
    int TEMPARR8[] = {BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR7, 1)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR8,
                               1)) {

            STREAM_BankOutputEvent_out *event_for_tout =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t tout = event_for_tout->cases.depositTo.timestamp;

            STREAM_BankOutputEvent_out *event_for_account =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int account = event_for_account->cases.depositTo.account;

            STREAM_BankInputEvent_out *event_for_tin =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t tin = event_for_tin->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            if (true) {

                if (tout == tin + 1) {
                    assert(action == 1);

                    current_rule_set = SWITCH_TO_RULE_SET_working;
                } else if (tout > tin) {
                    ++in_processed;

                    shm_arbiter_buffer_drop(BUFFER_In, 1);
                } else {
                    /* the event is incomplete, there must have been some hole,
                       try once more */
                    assert(in_holes > 0);
                    ++out_processed;

                    shm_arbiter_buffer_drop(BUFFER_Out, 1);
                    current_rule_set = SWITCH_TO_RULE_SET_aligning;
                }

                return 1;
            }
        }
    }

    int TEMPARR9[]  = {BANKOUTPUTEVENT_WITHDRAW};
    int TEMPARR10[] = {BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR9, 1)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR10,
                               1)) {

            STREAM_BankOutputEvent_out *event_for_tout =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t tout = event_for_tout->cases.withdraw.timestamp;

            STREAM_BankOutputEvent_out *event_for_account =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int account = event_for_account->cases.withdraw.account;

            STREAM_BankInputEvent_out *event_for_tin =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t tin = event_for_tin->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            if (true) {

                if (tout == tin + 1) {
                    assert(action == 2);

                    current_rule_set = SWITCH_TO_RULE_SET_working;
                } else if (tout > tin) {
                    ++in_processed;

                    shm_arbiter_buffer_drop(BUFFER_In, 1);
                } else {
                    /* the event is incomplete, there must have been some hole,
                       try once more */
                    assert(in_holes > 0);
                    ++out_processed;

                    shm_arbiter_buffer_drop(BUFFER_Out, 1);
                    current_rule_set = SWITCH_TO_RULE_SET_aligning;
                }

                return 1;
            }
        }
    }

    int TEMPARR11[] = {BANKOUTPUTEVENT_TRANSFER};
    int TEMPARR12[] = {BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR11, 1)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR12,
                               1)) {

            STREAM_BankOutputEvent_out *event_for_tout =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t tout = event_for_tout->cases.transfer.timestamp;

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

            STREAM_BankInputEvent_out *event_for_tin =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t tin = event_for_tin->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            if (true) {

                if (tout == tin + 4) {
                    assert(action == 4);

                    current_rule_set = SWITCH_TO_RULE_SET_working;
                } else if (tout > tin + 4) {
                    ++in_processed;

                    shm_arbiter_buffer_drop(BUFFER_In, 1);
                } else {
                    /* the event is incomplete, there must have been some hole,
                       try once more */
                    assert(in_holes > 0);
                    ++out_processed;

                    shm_arbiter_buffer_drop(BUFFER_Out, 1);
                    current_rule_set = SWITCH_TO_RULE_SET_aligning;
                }

                return 1;
            }
        }
    }

    int TEMPARR13[] = {BANKOUTPUTEVENT_LOGOUT};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR13, 1)) {

        STREAM_BankOutputEvent_out *event_for_t =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t = event_for_t->cases.logout.timestamp;

        if (true) {

            assert(0 && "Should not happen");

            return 1;
        }
    }

    int TEMPARR14[] = {BANKOUTPUTEVENT_NUMOUT};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR14, 1)) {

        STREAM_BankOutputEvent_out *event_for_t =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t = event_for_t->cases.numOut.timestamp;

        STREAM_BankOutputEvent_out *event_for_n =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int n = event_for_n->cases.numOut.num;

        if (true) {

            assert(0 && "Should not happen");

            return 1;
        }
    }

    int TEMPARR15[] = {BANKOUTPUTEVENT_SELECTEDACCOUNT};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR15, 1)) {

        STREAM_BankOutputEvent_out *event_for_t =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t = event_for_t->cases.selectedAccount.timestamp;

        STREAM_BankOutputEvent_out *event_for_a =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int a = event_for_a->cases.selectedAccount.account;

        if (true) {

            assert(0 && "Should not happen");

            return 1;
        }
    }

    int TEMPARR16[] = {BANKOUTPUTEVENT_DEPOSITSUCCESS};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR16, 1)) {

        STREAM_BankOutputEvent_out *event_for_t =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t = event_for_t->cases.depositSuccess.timestamp;

        if (true) {

            assert(0 && "Should not happen");

            return 1;
        }
    }

    int TEMPARR17[] = {BANKOUTPUTEVENT_DEPOSITFAIL};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR17, 1)) {

        STREAM_BankOutputEvent_out *event_for_t =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t = event_for_t->cases.depositFail.timestamp;

        if (true) {

            assert(0 && "Should not happen");

            return 1;
        }
    }

    int TEMPARR18[] = {BANKOUTPUTEVENT_WITHDRAWSUCCESS};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR18, 1)) {

        STREAM_BankOutputEvent_out *event_for_t =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t = event_for_t->cases.withdrawSuccess.timestamp;

        if (true) {

            assert(0 && "Should not happen");

            return 1;
        }
    }

    int TEMPARR19[] = {BANKOUTPUTEVENT_WITHDRAWFAIL};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR19, 1)) {

        STREAM_BankOutputEvent_out *event_for_t =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t = event_for_t->cases.withdrawFail.timestamp;

        if (true) {

            assert(0 && "Should not happen");

            return 1;
        }
    }

    int TEMPARR20[] = {BANKOUTPUTEVENT_TRANSFERSUCCESS};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR20, 1)) {

        STREAM_BankOutputEvent_out *event_for_t =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t = event_for_t->cases.transferSuccess.timestamp;

        if (true) {

            assert(0 && "Should not happen");

            return 1;
        }
    }

    int TEMPARR21[] = {BANKINPUTEVENT_HOLE};

    if (check_at_least_n_events(count_Out, 1)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR21,
                               1)) {

            STREAM_BankInputEvent_out *event_for_n =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int n = event_for_n->cases.hole.n;

            if (true) {

                arbiter_outevent =
                    (STREAM_BankEvent_out *)shm_monitor_buffer_write_ptr(
                        monitor_buffer);
                arbiter_outevent->head.kind = 6;
                arbiter_outevent->head.id   = (*arbiter_counter)++;

                shm_monitor_buffer_write_finish(monitor_buffer);

                ++in_holes;
                ++in_processed;

                shm_arbiter_buffer_drop(BUFFER_In, 1);

                return 1;
            }
        }
    }

    int TEMPARR22[] = {BANKINPUTEVENT_OTHERIN};

    if (check_at_least_n_events(count_Out, 1)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR22,
                               1)) {

            STREAM_BankInputEvent_out *event_for_tin =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t tin = event_for_tin->cases.otherIn.timestamp;

            if (true) {

                shm_arbiter_buffer_drop(BUFFER_In, 1);

                return 1;
            }
        }
    }

    if (count_Out == 0 && is_stream_done(EV_SOURCE_Out)) {
        if (check_at_least_n_events(count_In, 1)) {

            if (true) {

                ++in_processed;

                shm_arbiter_buffer_drop(BUFFER_In, 1);

                return 1;
            }
        }
    }

    if (check_at_least_n_events(count_Out, 1)) {
        if (count_In == 0 && is_stream_done(EV_SOURCE_In)) {

            if (true) {

                ++out_processed;

                shm_arbiter_buffer_drop(BUFFER_Out, 1);
                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                return 1;
            }
        }
    }
    _Bool ok = 1;
    if (count_Out >= 1) {
        if (count_In >= 1) {
            ok = 0;
        }
    }
    if (ok == 0) {
        fprintf(stderr, "Prefix of 'Out':\n");
        count_Out = shm_arbiter_buffer_peek(BUFFER_Out, 5, &e1_Out, &i1_Out,
                                            &e2_Out, &i2_Out);
        print_buffer_prefix(BUFFER_Out, 1, i1_Out + i2_Out, count_Out, e1_Out,
                            i1_Out, e2_Out, i2_Out);
        fprintf(stderr, "Prefix of 'In':\n");
        count_In = shm_arbiter_buffer_peek(BUFFER_In, 5, &e1_In, &i1_In, &e2_In,
                                           &i2_In);
        print_buffer_prefix(BUFFER_In, 0, i1_In + i2_In, count_In, e1_In, i1_In,
                            e2_In, i2_In);
        fprintf(stderr,
                "No rule in rule set '{rule_set_name}' matched even though "
                "there was enough events, CYCLING WITH NO PROGRESS!\n");
        abort();
    }
    if (++RULE_SET_align_in_nomatch_cnt > 8000000) {
        RULE_SET_align_in_nomatch_cnt = 0;
        fprintf(stderr, "\033[31mRule set 'align_in' cycles long time without "
                        "progress\033[0m\n");
        fprintf(stderr, "Prefix of 'Out':\n");
        count_Out = shm_arbiter_buffer_peek(BUFFER_Out, 5, &e1_Out, &i1_Out,
                                            &e2_Out, &i2_Out);
        print_buffer_prefix(BUFFER_Out, 1, i1_Out + i2_Out, count_Out, e1_Out,
                            i1_Out, e2_Out, i2_Out);
        fprintf(stderr, "Prefix of 'In':\n");
        count_In = shm_arbiter_buffer_peek(BUFFER_In, 5, &e1_In, &i1_In, &e2_In,
                                           &i2_In);
        print_buffer_prefix(BUFFER_In, 0, i1_In + i2_In, count_In, e1_In, i1_In,
                            e2_In, i2_In);
        fprintf(stderr, "Seems all rules are waiting for some events that are "
                        "not coming\n");
    }
    return 0;
}
int RULE_SET_working() {
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

    int TEMPARR23[] = {BANKOUTPUTEVENT_TRANSFER,
                       BANKOUTPUTEVENT_TRANSFERSUCCESS};
    int TEMPARR24[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_NUMIN,
                       BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR23, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR24,
                               3)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.transfer.timestamp;

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

            STREAM_BankOutputEvent_out *event_for_t4 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            uint64_t t4 = event_for_t4->cases.transferSuccess.timestamp;

            STREAM_BankInputEvent_out *event_for_t0 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t t0 = event_for_t0->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_t1 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            uint64_t t1 = event_for_t1->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_acc =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            int acc = event_for_acc->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_t3 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 2);
            uint64_t t3 = event_for_t3->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_amnt =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 2);
            int amnt = event_for_amnt->cases.numIn.num;

            if (true) {

                assert(action == 4);

                arbiter_outevent =
                    (STREAM_BankEvent_out *)shm_monitor_buffer_write_ptr(
                        monitor_buffer);
                arbiter_outevent->head.kind = 5;
                arbiter_outevent->head.id   = (*arbiter_counter)++;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.montransfer.from_ = from_;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.montransfer.to_ = to_;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.montransfer.amount = amnt;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.montransfer.success = true;

                shm_monitor_buffer_write_finish(monitor_buffer);
                out_processed += 2;
                in_processed += 3;

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 3);

                return 1;
            }
        }
    }

    int TEMPARR25[] = {BANKOUTPUTEVENT_TRANSFER,
                       BANKOUTPUTEVENT_TRANSFERSUCCESS};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR25, 2)) {
        if (check_at_least_n_events(count_In, 3)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.transfer.timestamp;

            STREAM_BankOutputEvent_out *event_for_f =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int f = event_for_f->cases.transfer.fromAccount;

            STREAM_BankOutputEvent_out *event_for_t =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int t = event_for_t->cases.transfer.toAccount;

            STREAM_BankOutputEvent_out *event_for_t4 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            uint64_t t4 = event_for_t4->cases.transferSuccess.timestamp;

            if (true) {

                /* some In event is not matching */
                in_processed += 2;
                out_processed += 2;

                shm_arbiter_buffer_drop(BUFFER_In, 1);
                /* drop the number */

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);

                return 1;
            }
        }
    }

    int TEMPARR26[] = {BANKOUTPUTEVENT_TRANSFER,
                       BANKOUTPUTEVENT_SELECTEDACCOUNT};
    int TEMPARR27[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_NUMIN,
                       BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR26, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR27,
                               3)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.transfer.timestamp;

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

            STREAM_BankOutputEvent_out *event_for_t4 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            uint64_t t4 = event_for_t4->cases.selectedAccount.timestamp;

            STREAM_BankInputEvent_out *event_for_acc =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            int acc = event_for_acc->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_t0 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t t0 = event_for_t0->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_t1 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            uint64_t t1 = event_for_t1->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_t3 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 2);
            uint64_t t3 = event_for_t3->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_amnt =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 2);
            int amnt = event_for_amnt->cases.numIn.num;

            if (true) {

                assert(action == 4);

                arbiter_outevent =
                    (STREAM_BankEvent_out *)shm_monitor_buffer_write_ptr(
                        monitor_buffer);
                arbiter_outevent->head.kind = 5;
                arbiter_outevent->head.id   = (*arbiter_counter)++;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.montransfer.from_ = from_;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.montransfer.to_ = to_;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.montransfer.amount = amnt;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.montransfer.success = false;

                shm_monitor_buffer_write_finish(monitor_buffer);
                out_processed += 2;
                in_processed += 3;

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 3);

                return 1;
            }
        }
    }

    int TEMPARR28[] = {BANKOUTPUTEVENT_TRANSFER,
                       BANKOUTPUTEVENT_SELECTEDACCOUNT};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR28, 2)) {
        if (check_at_least_n_events(count_In, 3)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.transfer.timestamp;

            STREAM_BankOutputEvent_out *event_for_f =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int f = event_for_f->cases.transfer.fromAccount;

            STREAM_BankOutputEvent_out *event_for_t =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int t = event_for_t->cases.transfer.toAccount;

            STREAM_BankOutputEvent_out *event_for_t4 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            uint64_t t4 = event_for_t4->cases.selectedAccount.timestamp;

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            int acc = event_for_acc->cases.selectedAccount.account;

            if (true) {

                /* some In event is not matching */
                in_processed += 2;
                out_processed += 2;

                shm_arbiter_buffer_drop(BUFFER_In, 1);
                /* drop the number */

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);

                return 1;
            }
        }
    }

    int TEMPARR29[] = {BANKOUTPUTEVENT_TRANSFER, BANKOUTPUTEVENT_HOLE};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR29, 2)) {

        STREAM_BankOutputEvent_out *event_for_t2 =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t2 = event_for_t2->cases.transfer.timestamp;

        STREAM_BankOutputEvent_out *event_for_f =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int f = event_for_f->cases.transfer.fromAccount;

        STREAM_BankOutputEvent_out *event_for_t =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int t = event_for_t->cases.transfer.toAccount;

        STREAM_BankOutputEvent_out *event_for_n =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int n = event_for_n->cases.hole.n;

        if (true) {

            ++out_processed;

            current_rule_set = SWITCH_TO_RULE_SET_aligning;

            shm_arbiter_buffer_drop(BUFFER_Out, 1);

            return 1;
        }
    }

    int TEMPARR30[] = {BANKOUTPUTEVENT_TRANSFER};
    int TEMPARR31[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_HOLE};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR30, 1)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR31,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.transfer.timestamp;

            STREAM_BankOutputEvent_out *event_for_f =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int f = event_for_f->cases.transfer.fromAccount;

            STREAM_BankOutputEvent_out *event_for_t =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int t = event_for_t->cases.transfer.toAccount;

            STREAM_BankInputEvent_out *event_for_t0 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t t0 = event_for_t0->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_n =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int n = event_for_n->cases.hole.n;

            if (true) {

                /* some In event is not matching */

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 1);
                shm_arbiter_buffer_drop(BUFFER_In, 1);

                return 1;
            }
        }
    }

    int TEMPARR32[] = {BANKOUTPUTEVENT_TRANSFER};
    int TEMPARR33[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_OTHERIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR32, 1)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR33,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.transfer.timestamp;

            STREAM_BankOutputEvent_out *event_for_f =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int f = event_for_f->cases.transfer.fromAccount;

            STREAM_BankOutputEvent_out *event_for_t =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int t = event_for_t->cases.transfer.toAccount;

            STREAM_BankInputEvent_out *event_for_t0 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t t0 = event_for_t0->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_t1 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            uint64_t t1 = event_for_t1->cases.otherIn.timestamp;

            if (true) {

                /* some In event is not matching */

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 1);
                shm_arbiter_buffer_drop(BUFFER_In, 2);

                return 1;
            }
        }
    }

    int TEMPARR34[] = {BANKOUTPUTEVENT_BALANCE, BANKOUTPUTEVENT_NUMOUT};
    int TEMPARR35[] = {BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR34, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR35,
                               1)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.balance.timestamp;

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int acc = event_for_acc->cases.balance.account;

            STREAM_BankOutputEvent_out *event_for_t3 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            uint64_t t3 = event_for_t3->cases.numOut.timestamp;

            STREAM_BankOutputEvent_out *event_for_amnt =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            int amnt = event_for_amnt->cases.numOut.num;

            STREAM_BankInputEvent_out *event_for_t1 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t t1 = event_for_t1->cases.numIn.timestamp;

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
                    ->cases.monbalance.account = acc;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.monbalance.balance = amnt;

                shm_monitor_buffer_write_finish(monitor_buffer);
                out_processed += 2;
                ++in_processed;

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 1);

                return 1;
            }
        }
    }

    int TEMPARR36[] = {BANKOUTPUTEVENT_BALANCE, BANKOUTPUTEVENT_HOLE};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR36, 2)) {

        STREAM_BankOutputEvent_out *event_for_t2 =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t2 = event_for_t2->cases.balance.timestamp;

        STREAM_BankOutputEvent_out *event_for_acc =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int acc = event_for_acc->cases.balance.account;

        STREAM_BankOutputEvent_out *event_for_n =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int n = event_for_n->cases.hole.n;

        if (true) {

            ++out_processed;

            current_rule_set = SWITCH_TO_RULE_SET_aligning;

            shm_arbiter_buffer_drop(BUFFER_Out, 1);

            return 1;
        }
    }

    int TEMPARR37[] = {BANKOUTPUTEVENT_DEPOSITTO,
                       BANKOUTPUTEVENT_DEPOSITSUCCESS};
    int TEMPARR38[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR37, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR38,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.depositTo.timestamp;

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int acc = event_for_acc->cases.depositTo.account;

            STREAM_BankOutputEvent_out *event_for_t4 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            uint64_t t4 = event_for_t4->cases.depositSuccess.timestamp;

            STREAM_BankInputEvent_out *event_for_t1 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t t1 = event_for_t1->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_t3 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            uint64_t t3 = event_for_t3->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_amnt =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            int amnt = event_for_amnt->cases.numIn.num;

            if (true) {

                assert(action == 1);
                assert(t2 == t1 + 1);
                assert(t3 == t2 + 1);

                arbiter_outevent =
                    (STREAM_BankEvent_out *)shm_monitor_buffer_write_ptr(
                        monitor_buffer);
                arbiter_outevent->head.kind = 3;
                arbiter_outevent->head.id   = (*arbiter_counter)++;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.deposit.account = acc;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.deposit.amount = amnt;

                shm_monitor_buffer_write_finish(monitor_buffer);
                out_processed += 2;
                in_processed += 2;

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 2);

                return 1;
            }
        }
    }

    int TEMPARR39[] = {BANKOUTPUTEVENT_DEPOSITTO, BANKOUTPUTEVENT_DEPOSITFAIL};
    int TEMPARR40[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR39, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR40,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.depositTo.timestamp;

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int acc = event_for_acc->cases.depositTo.account;

            STREAM_BankOutputEvent_out *event_for_t4 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            uint64_t t4 = event_for_t4->cases.depositFail.timestamp;

            STREAM_BankInputEvent_out *event_for_t1 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t t1 = event_for_t1->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_t3 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            uint64_t t3 = event_for_t3->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_amnt =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            int amnt = event_for_amnt->cases.numIn.num;

            if (true) {

                assert(action == 1);
                out_processed += 2;
                in_processed += 2;

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 2);

                return 1;
            }
        }
    }

    int TEMPARR41[] = {BANKOUTPUTEVENT_DEPOSITTO, BANKOUTPUTEVENT_DEPOSITFAIL};
    int TEMPARR42[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_HOLE};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR41, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR42,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.depositTo.timestamp;

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int acc = event_for_acc->cases.depositTo.account;

            STREAM_BankOutputEvent_out *event_for_t4 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            uint64_t t4 = event_for_t4->cases.depositFail.timestamp;

            STREAM_BankInputEvent_out *event_for_t1 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t t1 = event_for_t1->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_n =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int n = event_for_n->cases.hole.n;

            if (true) {

                assert(action == 1);
                out_processed += 2;
                in_processed += 1;

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 1);

                return 1;
            }
        }
    }

    int TEMPARR43[] = {BANKOUTPUTEVENT_DEPOSITTO,
                       BANKOUTPUTEVENT_DEPOSITSUCCESS};
    int TEMPARR44[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_HOLE};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR43, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR44,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.depositTo.timestamp;

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int acc = event_for_acc->cases.depositTo.account;

            STREAM_BankOutputEvent_out *event_for_t4 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            uint64_t t4 = event_for_t4->cases.depositSuccess.timestamp;

            STREAM_BankInputEvent_out *event_for_t1 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t t1 = event_for_t1->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_action =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int action = event_for_action->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_n =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int n = event_for_n->cases.hole.n;

            if (true) {

                assert(action == 1);
                out_processed += 2;
                in_processed += 1;

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 1);

                return 1;
            }
        }
    }

    int TEMPARR45[] = {BANKOUTPUTEVENT_DEPOSITTO, BANKOUTPUTEVENT_HOLE};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR45, 2)) {

        STREAM_BankOutputEvent_out *event_for_t2 =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t2 = event_for_t2->cases.depositTo.timestamp;

        STREAM_BankOutputEvent_out *event_for_acc =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int acc = event_for_acc->cases.depositTo.account;

        STREAM_BankOutputEvent_out *event_for_n =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int n = event_for_n->cases.hole.n;

        if (true) {

            ++out_processed;

            current_rule_set = SWITCH_TO_RULE_SET_aligning;

            shm_arbiter_buffer_drop(BUFFER_Out, 1);

            return 1;
        }
    }

    int TEMPARR46[] = {BANKOUTPUTEVENT_WITHDRAW,
                       BANKOUTPUTEVENT_WITHDRAWSUCCESS};
    int TEMPARR47[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR46, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR47,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.withdraw.timestamp;

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int acc = event_for_acc->cases.withdraw.account;

            STREAM_BankOutputEvent_out *event_for_t4 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            uint64_t t4 = event_for_t4->cases.withdrawSuccess.timestamp;

            STREAM_BankInputEvent_out *event_for_t1 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t t1 = event_for_t1->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_act =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int act = event_for_act->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_t3 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            uint64_t t3 = event_for_t3->cases.numIn.timestamp;

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
                    ->cases.monwithdraw.account = acc;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.monwithdraw.amount = amnt;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.monwithdraw.success = true;

                shm_monitor_buffer_write_finish(monitor_buffer);
                out_processed += 2;
                in_processed += 2;

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 2);

                return 1;
            }
        }
    }

    int TEMPARR48[] = {BANKOUTPUTEVENT_WITHDRAW, BANKOUTPUTEVENT_WITHDRAWFAIL};
    int TEMPARR49[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_NUMIN};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR48, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR49,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.withdraw.timestamp;

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int acc = event_for_acc->cases.withdraw.account;

            STREAM_BankOutputEvent_out *event_for_t4 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            uint64_t t4 = event_for_t4->cases.withdrawFail.timestamp;

            STREAM_BankInputEvent_out *event_for_t1 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t t1 = event_for_t1->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_act =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int act = event_for_act->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_t3 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 1);
            uint64_t t3 = event_for_t3->cases.numIn.timestamp;

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
                    ->cases.monwithdraw.account = acc;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.monwithdraw.amount = amnt;
                ((STREAM_BankEvent_out *)arbiter_outevent)
                    ->cases.monwithdraw.success = false;

                shm_monitor_buffer_write_finish(monitor_buffer);
                out_processed += 2;
                in_processed += 2;

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 2);

                return 1;
            }
        }
    }

    int TEMPARR50[] = {BANKOUTPUTEVENT_WITHDRAW,
                       BANKOUTPUTEVENT_WITHDRAWSUCCESS};
    int TEMPARR51[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_HOLE};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR50, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR51,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.withdraw.timestamp;

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int acc = event_for_acc->cases.withdraw.account;

            STREAM_BankOutputEvent_out *event_for_t4 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            uint64_t t4 = event_for_t4->cases.withdrawSuccess.timestamp;

            STREAM_BankInputEvent_out *event_for_t1 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t t1 = event_for_t1->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_act =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int act = event_for_act->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_n =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int n = event_for_n->cases.hole.n;

            if (true) {

                out_processed += 2;
                in_processed += 1;

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 1);

                return 1;
            }
        }
    }

    int TEMPARR52[] = {BANKOUTPUTEVENT_WITHDRAW, BANKOUTPUTEVENT_WITHDRAWFAIL};
    int TEMPARR53[] = {BANKINPUTEVENT_NUMIN, BANKINPUTEVENT_HOLE};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR52, 2)) {

        if (are_events_in_head(e1_In, i1_In, e2_In, i2_In, count_In,
                               sizeof(STREAM_BankInputEvent_out), TEMPARR53,
                               2)) {

            STREAM_BankOutputEvent_out *event_for_t2 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            uint64_t t2 = event_for_t2->cases.withdraw.timestamp;

            STREAM_BankOutputEvent_out *event_for_acc =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 0);
            int acc = event_for_acc->cases.withdraw.account;

            STREAM_BankOutputEvent_out *event_for_t4 =
                (STREAM_BankOutputEvent_out *)get_event_at_index(
                    e1_Out, i1_Out, e2_Out, i2_Out,
                    sizeof(STREAM_BankOutputEvent_out), 1);
            uint64_t t4 = event_for_t4->cases.withdrawFail.timestamp;

            STREAM_BankInputEvent_out *event_for_t1 =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            uint64_t t1 = event_for_t1->cases.numIn.timestamp;

            STREAM_BankInputEvent_out *event_for_act =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int act = event_for_act->cases.numIn.num;

            STREAM_BankInputEvent_out *event_for_n =
                (STREAM_BankInputEvent_out *)get_event_at_index(
                    e1_In, i1_In, e2_In, i2_In,
                    sizeof(STREAM_BankInputEvent_out), 0);
            int n = event_for_n->cases.hole.n;

            if (true) {

                out_processed += 2;
                in_processed += 1;

                current_rule_set = SWITCH_TO_RULE_SET_aligning;

                shm_arbiter_buffer_drop(BUFFER_Out, 2);
                shm_arbiter_buffer_drop(BUFFER_In, 1);

                return 1;
            }
        }
    }

    int TEMPARR54[] = {BANKOUTPUTEVENT_WITHDRAW, BANKOUTPUTEVENT_HOLE};

    if (are_events_in_head(e1_Out, i1_Out, e2_Out, i2_Out, count_Out,
                           sizeof(STREAM_BankOutputEvent_out), TEMPARR54, 2)) {

        STREAM_BankOutputEvent_out *event_for_t2 =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        uint64_t t2 = event_for_t2->cases.withdraw.timestamp;

        STREAM_BankOutputEvent_out *event_for_acc =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int acc = event_for_acc->cases.withdraw.account;

        STREAM_BankOutputEvent_out *event_for_n =
            (STREAM_BankOutputEvent_out *)get_event_at_index(
                e1_Out, i1_Out, e2_Out, i2_Out,
                sizeof(STREAM_BankOutputEvent_out), 0);
        int n = event_for_n->cases.hole.n;

        if (true) {

            ++out_processed;

            current_rule_set = SWITCH_TO_RULE_SET_aligning;

            shm_arbiter_buffer_drop(BUFFER_Out, 1);

            return 1;
        }
    }

    if (count_In == 0 && is_stream_done(EV_SOURCE_In)) {

        if (true) {

            current_rule_set = SWITCH_TO_RULE_SET_aligning;

            return 1;
        }
    }

    if (count_Out == 0 && is_stream_done(EV_SOURCE_Out)) {

        if (true) {

            current_rule_set = SWITCH_TO_RULE_SET_aligning;

            return 1;
        }
    }

    _Bool ok = 1;
    if (count_Out >= 2) {
        if (count_In >= 3) {
            ok = 0;
        }
    }
    if (ok == 0) {
        fprintf(stderr, "Prefix of 'Out':\n");
        count_Out = shm_arbiter_buffer_peek(BUFFER_Out, 5, &e1_Out, &i1_Out,
                                            &e2_Out, &i2_Out);
        print_buffer_prefix(BUFFER_Out, 1, i1_Out + i2_Out, count_Out, e1_Out,
                            i1_Out, e2_Out, i2_Out);
        fprintf(stderr, "Prefix of 'In':\n");
        count_In = shm_arbiter_buffer_peek(BUFFER_In, 5, &e1_In, &i1_In, &e2_In,
                                           &i2_In);
        print_buffer_prefix(BUFFER_In, 0, i1_In + i2_In, count_In, e1_In, i1_In,
                            e2_In, i2_In);
        fprintf(stderr,
                "No rule in rule set '{rule_set_name}' matched even though "
                "there was enough events, CYCLING WITH NO PROGRESS!\n");
        abort();
    }
    if (++RULE_SET_working_nomatch_cnt > 8000000) {
        RULE_SET_working_nomatch_cnt = 0;
        fprintf(stderr, "\033[31mRule set 'working' cycles long time without "
                        "progress\033[0m\n");
        fprintf(stderr, "Prefix of 'Out':\n");
        count_Out = shm_arbiter_buffer_peek(BUFFER_Out, 5, &e1_Out, &i1_Out,
                                            &e2_Out, &i2_Out);
        print_buffer_prefix(BUFFER_Out, 1, i1_Out + i2_Out, count_Out, e1_Out,
                            i1_Out, e2_Out, i2_Out);
        fprintf(stderr, "Prefix of 'In':\n");
        count_In = shm_arbiter_buffer_peek(BUFFER_In, 5, &e1_In, &i1_In, &e2_In,
                                           &i2_In);
        print_buffer_prefix(BUFFER_In, 0, i1_In + i2_In, count_In, e1_In, i1_In,
                            e2_In, i2_In);
        fprintf(stderr, "Seems all rules are waiting for some events that are "
                        "not coming\n");
    }
    return 0;
}
int arbiter() {

    while (!are_streams_done()) {
        int rule_sets_match_count = 0;
        if (current_rule_set == SWITCH_TO_RULE_SET_aligning) {
            rule_sets_match_count += RULE_SET_aligning();
        }
        if (current_rule_set == SWITCH_TO_RULE_SET_align_in) {
            rule_sets_match_count += RULE_SET_align_in();
        }
        if (current_rule_set == SWITCH_TO_RULE_SET_working) {
            rule_sets_match_count += RULE_SET_working();
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
            printf("in_holes: %i, out_holes: %i\n", in_holes, out_holes);
            printf("Errors found: %d\n", errors);
            fflush(stdout);
            destroy_intmap(&balances);
            destroy_intmap(&upper_bounds);
            destroy_intmap(&lower_bounds);
            fclose(tessla_in);

            abort();
        }
    }
    shm_monitor_set_finished(monitor_buffer);
}

static void sig_handler(int sig) {
    printf("signal %d caught...", sig);
    shm_stream_detach(EV_SOURCE_In);
    shm_stream_detach(EV_SOURCE_Out);
    __work_done = 1;
}

static void setup_signals() {
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        perror("failed setting SIGINT handler");
    }

    if (signal(SIGABRT, sig_handler) == SIG_ERR) {
        perror("failed setting SIGINT handler");
    }

    if (signal(SIGIOT, sig_handler) == SIG_ERR) {
        perror("failed setting SIGINT handler");
    }

    if (signal(SIGSEGV, sig_handler) == SIG_ERR) {
        perror("failed setting SIGINT handler");
    }
}

int main(int argc, char **argv) {
    setup_signals();

    chosen_streams = (dll_node *)malloc(2); // the maximum size this can have is
                                            // the total number of event sources
    arbiter_counter  = malloc(sizeof(int));
    *arbiter_counter = 10;
    init_intmap(&balances);
    init_intmap(&upper_bounds);
    init_intmap(&lower_bounds);
    tessla_in = fopen("/tmp/tessla.in", "w");
    if (tessla_in == NULL) {
        fprintf(stderr, "Failed openning TeSSLa fifo\n");
        abort();
    }

    // connect to event source In
    EV_SOURCE_In = shm_stream_create_from_argv("In", argc, argv);
    BUFFER_In    = shm_arbiter_buffer_create(
           EV_SOURCE_In, sizeof(STREAM_BankInputEvent_out), 10000);

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
           EV_SOURCE_Out, sizeof(STREAM_BankOutputEvent_out), 10000);

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
    if (shm_stream_register_event(EV_SOURCE_Out, "depositFail",
                                  BANKOUTPUTEVENT_DEPOSITFAIL) < 0) {
        fprintf(stderr, "Failed registering event depositFail for stream Out : "
                        "BankOutputEvent\n");
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
    if (shm_stream_register_event(EV_SOURCE_Out, "logout",
                                  BANKOUTPUTEVENT_LOGOUT) < 0) {
        fprintf(stderr, "Failed registering event logout for stream Out : "
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

    monitor_buffer =
        shm_monitor_buffer_create(sizeof(STREAM_BankEvent_out), 10000);

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

        if (received_event->head.kind == 2) {
            int account = received_event->cases.monbalance.account;
            int balance = received_event->cases.monbalance.balance;

            if (true) {
                fprintf(tessla_in, "%lu: monbalance = (%d, %d)\n", ++timestamp,
                        account, balance);
            }
        }

        if (received_event->head.kind == 3) {
            int account = received_event->cases.deposit.account;
            int amount  = received_event->cases.deposit.amount;

            if (true) {
                fprintf(tessla_in, "%lu: deposit = (%d, %d)\n", ++timestamp,
                        account, amount);
            }
        }

        if (received_event->head.kind == 4) {
            int  account = received_event->cases.monwithdraw.account;
            int  amount  = received_event->cases.monwithdraw.amount;
            bool success = received_event->cases.monwithdraw.success;

            if (true) {
                fprintf(tessla_in, "%lu: monwithdraw = (%d, %d, %d)\n",
                        ++timestamp, account, amount, success);
            }
        }

        if (received_event->head.kind == 5) {
            int  from_   = received_event->cases.montransfer.from_;
            int  to_     = received_event->cases.montransfer.to_;
            int  amount  = received_event->cases.montransfer.amount;
            bool success = received_event->cases.montransfer.success;

            if (true) {
                fprintf(tessla_in, "%lu: montransfer = (%d, %d, %d, %d)\n",
                        ++timestamp, from_, to_, amount, success);
            }
        }

        if (received_event->head.kind == 6) {

            if (true) {
                fprintf(tessla_in, "%lu: reset = ()\n", ++timestamp);
            }
        }

        if (received_event->head.kind == 1) {
            int n = received_event->cases.hole.n;

            if (true) {
                fprintf(tessla_in, "THERE IS A HOLE\n");
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
    printf("in_holes: %i, out_holes: %i\n", in_holes, out_holes);
    printf("Errors found: %d\n", errors);
    fflush(stdout);
    destroy_intmap(&balances);
    destroy_intmap(&upper_bounds);
    destroy_intmap(&lower_bounds);
    fclose(tessla_in);
}
