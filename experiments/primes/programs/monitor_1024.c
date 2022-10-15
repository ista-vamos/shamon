#include "shamon.h"
#include "mmlib.h"
#include "monitor.h"
#include "./compiler/cfiles/compiler_utils.h"
#include <threads.h>
#include <signal.h>
#include <stdio.h>
#include <stdatomic.h>

struct _EVENT_hole
{
  uint64_t n;
};
typedef struct _EVENT_hole EVENT_hole;
// event declarations for stream type Primes
struct _EVENT_Prime {
	int n;
	int p;

};
typedef struct _EVENT_Prime EVENT_Prime;

// input stream for stream type Primes
struct _STREAM_Primes_in {
    shm_event head;
    union {
        EVENT_Prime Prime;
    }cases;
};
typedef struct _STREAM_Primes_in STREAM_Primes_in;

// output stream for stream type Primes
struct _STREAM_Primes_out {
    shm_event head;
    union {
        EVENT_hole hole;
        EVENT_Prime Prime;
    }cases;
};
typedef struct _STREAM_Primes_out STREAM_Primes_out;
        // event declarations for stream type NumberPairs
struct _EVENT_NumberPair {
	int i;
	int n;
	int m;

};
typedef struct _EVENT_NumberPair EVENT_NumberPair;

// input stream for stream type NumberPairs
struct _STREAM_NumberPairs_in {
    shm_event head;
    union {
        EVENT_NumberPair NumberPair;
    }cases;
};
typedef struct _STREAM_NumberPairs_in STREAM_NumberPairs_in;

// output stream for stream type NumberPairs
struct _STREAM_NumberPairs_out {
    shm_event head;
    union {
        EVENT_hole hole;
        EVENT_NumberPair NumberPair;
    }cases;
};
typedef struct _STREAM_NumberPairs_out STREAM_NumberPairs_out;
        

    typedef struct _STREAM_Primes_ARGS {
    	int pos;

    } STREAM_Primes_ARGS;
            
enum Primes_kinds {
PRIMES_PRIME = 2,
PRIMES_HOLE = 1,
};

STREAM_Primes_ARGS *stream_args_P_0;
STREAM_Primes_ARGS *stream_args_P_1;

int *arbiter_counter;
// monitor buffer
shm_monitor_buffer *monitor_buffer;

bool is_selection_successful;
dll_node **chosen_streams; // used in rule set for get_first/last_n

// globals code
STREAM_NumberPairs_out *arbiter_outevent;

#include "./compiler/cfiles/intmap.h"
     intmap buf;
     int count = 0;
 
     int left_drops = 0;
     int right_drops = 0;
     
     int left_processed = 0;
     int right_processed = 0;
 
     int compared = 0;
     
     int left_holes = 0;
     int right_holes = 0;
 
     int process(int n, int p, int pos, int opos)
     {
         if(pos < opos)
         {
             int oval=0;
             if(intmap_get(&buf, n, &oval))
             {
                 compared++;
                 

        arbiter_outevent = (STREAM_NumberPairs_out *)shm_monitor_buffer_write_ptr(monitor_buffer);
         arbiter_outevent->head.kind = 2;
    arbiter_outevent->head.id = (*arbiter_counter)++;
    ((STREAM_NumberPairs_out *) arbiter_outevent)->cases.NumberPair.i = n;
((STREAM_NumberPairs_out *) arbiter_outevent)->cases.NumberPair.n = oval;
((STREAM_NumberPairs_out *) arbiter_outevent)->cases.NumberPair.m = p;

         shm_monitor_buffer_write_finish(monitor_buffer);
        }
             int removed=intmap_remove_upto(&buf, n);
             count -= removed;
         }
         else
         {
             if(count<10)
             {
                 intmap_insert(&buf, n, p);
                 count++;
             }
         }
         return n+1;
     }
 
bool SHOULD_KEEP_P(shm_stream * s, shm_event * e) {
    return true;
}
            

atomic_int count_event_streams = 2;

// declare event streams
shm_stream *EV_SOURCE_P_0;
shm_stream *EV_SOURCE_P_1;


// event sources threads
thrd_t THREAD_P_0;
thrd_t THREAD_P_1;


// declare arbiter thread
thrd_t ARBITER_THREAD;
const int SWITCH_TO_RULE_SET_rs = 0;

static size_t RULE_SET_rs_nomatch_cnt = 0;

int current_rule_set = SWITCH_TO_RULE_SET_rs;

// Arbiter buffer for event source P (0)
shm_arbiter_buffer *BUFFER_P0;

// Arbiter buffer for event source P (1)
shm_arbiter_buffer *BUFFER_P1;




// buffer groups

bool Ps_ORDER_EXP (void *args1, void *args2) {
    return ((STREAM_Primes_ARGS *) args1)->pos > ((STREAM_Primes_ARGS *) args2)->pos;
}        


buffer_group BG_Ps;
        

int PERF_LAYER_P (shm_arbiter_buffer *buffer) {
    shm_stream *stream = shm_arbiter_buffer_stream(buffer);   
    STREAM_Primes_in *inevent;
    STREAM_Primes_out *outevent;   

    // wait for active buffer
    while ((!shm_arbiter_buffer_active(buffer))){
        sleep_ns(10);
    }
    while(true) {
        inevent = stream_filter_fetch(stream, buffer, &SHOULD_KEEP_P);
        
        if (inevent == NULL) {
            // no more events
            break;
        }
        outevent = shm_arbiter_buffer_write_ptr(buffer);
        
        memcpy(outevent, inevent, sizeof(STREAM_Primes_out));
        shm_arbiter_buffer_write_finish(buffer);
        shm_stream_consume(stream, 1);
    }  
    atomic_fetch_add(&count_event_streams, -1);   
}


// variables used to debug arbiter
long unsigned no_consecutive_matches_limit = 1UL<<35;
int no_matches_count = 0;

bool are_there_events(shm_arbiter_buffer * b) {
  return shm_arbiter_buffer_size(b) > 0;
}


bool are_buffers_empty() {
	int c = 0;
	c += are_there_events(BUFFER_P0);
	c += are_there_events(BUFFER_P1);

    return c == 0;
}
    

static int __work_done = 0;
/* TODO: make a keywork from this */
void done() {
    __work_done = 1;
}

static inline bool are_streams_done() {
    assert(count_event_streams >=0);
    return count_event_streams == 0 && are_buffers_empty() || __work_done;
}

static inline bool is_stream_done(shm_stream *s) {
    return !shm_stream_is_ready(s);
}


static inline
bool check_at_least_n_events(size_t count, size_t n) {
    // count is the result after calling shm_arbiter_buffer_peek
	return count >= n;
}

static
bool are_events_in_head(char* e1, size_t i1, char* e2, size_t i2, int count, size_t ev_size, int event_kinds[], int n_events) {
    assert(n_events > 0);
	if (count < n_events) {
	    return false;
    }

	int i = 0;
	while (i < i1) {
	    shm_event * ev = (shm_event *) (e1);
	     if (ev->kind != event_kinds[i]) {
	        return false;
	    }
        if (--n_events == 0)
            return true;
	    i+=1;
	    e1 += ev_size;
	}

	i = 0;
	while (i < i2) {
	    shm_event * ev = (shm_event *) e2;
	     if (ev->kind != event_kinds[i1+i]) {
	        return false;
	    }
        if (--n_events == 0)
            return true;
	    i+=1;
	    e2 += ev_size;
	}

	return true;
}

static void
print_buffer_prefix(shm_arbiter_buffer *b, size_t n_events, int cnt, char* e1, size_t i1, char* e2, size_t i2) {
    if (cnt == 0) {
        fprintf(stderr, " empty\n");
        return;
    }
    const size_t ev_size = shm_arbiter_buffer_elem_size(b);
    int n = 0;
	int i = 0;
	while (i < i1) {
	    shm_event * ev = (shm_event *) (e1);
        fprintf(stderr, "  %d: {id: %lu, kind: %lu}\n", ++n,
                shm_event_id(ev), shm_event_kind(ev));
        if (--n_events == 0)
            return;
	    i+=1;
	    e1 += ev_size;
	}

	i = 0;
	while (i < i2) {
	    shm_event * ev = (shm_event *) e2;
        fprintf(stderr, "  %d: {id: %lu, kind: %lu}\n", ++n,
                shm_event_id(ev), shm_event_kind(ev));
        if (--n_events == 0)
            return;
	    i+=1;
	    e2 += ev_size;
	}
}


static inline
shm_event * get_event_at_index(char* e1, size_t i1, char* e2, size_t i2, size_t size_event, int element_index) {
	if (element_index < i1) {
		return (shm_event *) (e1 + (element_index*size_event));
	} else {
		element_index -=i1;
		return (shm_event *) (e2 + (element_index*size_event));
	}
}

//arbiter outevent
STREAM_NumberPairs_out *arbiter_outevent;
int RULE_SET_rs();



void print_event_name(int ev_src_index, int event_index) {
    if (event_index == -1) {
        printf("None\n");
        return;
    }
    
    if (event_index == 1) {
        printf("hole\n");
        return;
    }
    
    
    if(ev_src_index == 0) {
        
        if (event_index == 2 ) {
            printf("Prime\n");
            return;
        }
            
        if (event_index == 1 ) {
            printf("hole\n");
            return;
        }
            
        printf("No event matched! this should not happen, please report!\n");
        return;
    }
        
    printf("Invalid event source! this should not happen, please report!\n");
}
    

int get_event_at_head(shm_arbiter_buffer *b) {
    void * e1; size_t i1;
    void * e2; size_t i2;
    
    int count = shm_arbiter_buffer_peek(b, 0, &e1, &i1, &e2, &i2);
    if (count == 0) {
        return -1;
    }
    shm_event * ev = (shm_event *) (e1);
    return ev->kind;
}
    

void print_buffers_state() {
    int event_index;
	event_index = get_event_at_head(BUFFER_P0);
	printf("P0 -> ");
	print_event_name(0, event_index);
	event_index = get_event_at_head(BUFFER_P1);
	printf("P1 -> ");
	print_event_name(0, event_index);

}    
    

int RULE_SET_rs() {
char* e1_P0; size_t i1_P0; char* e2_P0; size_t i2_P0;
int count_P0 = shm_arbiter_buffer_peek(BUFFER_P0, 1, (void**)&e1_P0, &i1_P0, (void**)&e2_P0, &i2_P0);
char* e1_P1; size_t i1_P1; char* e2_P1; size_t i2_P1;
int count_P1 = shm_arbiter_buffer_peek(BUFFER_P1, 1, (void**)&e1_P1, &i1_P1, (void**)&e2_P1, &i2_P1);

            int TEMPARR0[] = {PRIMES_PRIME};
int TEMPARR1[] = {PRIMES_PRIME};

            
                if (are_events_in_head(e1_P0, i1_P0, e2_P0, i2_P0, 
                count_P0, sizeof(STREAM_Primes_out), TEMPARR0, 1)) {
                    
                if (are_events_in_head(e1_P1, i1_P1, e2_P1, i2_P1, 
                count_P1, sizeof(STREAM_Primes_out), TEMPARR1, 1)) {
                    
            STREAM_Primes_out * event_for_ln = (STREAM_Primes_out *) get_event_at_index(e1_P0, i1_P0, e2_P0, i2_P0, sizeof(STREAM_Primes_out), 0);
int ln = event_for_ln->cases.Prime.n;

STREAM_Primes_out * event_for_lp = (STREAM_Primes_out *) get_event_at_index(e1_P0, i1_P0, e2_P0, i2_P0, sizeof(STREAM_Primes_out), 0);
int lp = event_for_lp->cases.Prime.p;

STREAM_Primes_out * event_for_rn = (STREAM_Primes_out *) get_event_at_index(e1_P1, i1_P1, e2_P1, i2_P1, sizeof(STREAM_Primes_out), 0);
int rn = event_for_rn->cases.Prime.n;

STREAM_Primes_out * event_for_rp = (STREAM_Primes_out *) get_event_at_index(e1_P1, i1_P1, e2_P1, i2_P1, sizeof(STREAM_Primes_out), 0);
int rp = event_for_rp->cases.Prime.p;


            if(true ) {
                
                if(ln == rn)
             {
                 left_processed += 1;
                 right_processed += 1;
                 compared++;
                 

        arbiter_outevent = (STREAM_NumberPairs_out *)shm_monitor_buffer_write_ptr(monitor_buffer);
         arbiter_outevent->head.kind = 2;
    arbiter_outevent->head.id = (*arbiter_counter)++;
    ((STREAM_NumberPairs_out *) arbiter_outevent)->cases.NumberPair.i = ln;
((STREAM_NumberPairs_out *) arbiter_outevent)->cases.NumberPair.n = lp;
((STREAM_NumberPairs_out *) arbiter_outevent)->cases.NumberPair.m = rp;

         shm_monitor_buffer_write_finish(monitor_buffer);
        	shm_arbiter_buffer_drop(BUFFER_P0, 1);
	shm_arbiter_buffer_drop(BUFFER_P1, 1);

int removed=intmap_clear(&buf);
                 count-=removed;
                 stream_args_P_0->pos
= ln + 1;
                 stream_args_P_1->pos
= rn + 1;
             }
             else if(stream_args_P_0->pos
<stream_args_P_1->pos
)
             {
                 left_processed += 1;
                 stream_args_P_0->pos
= process(ln, lp, stream_args_P_0->pos
, stream_args_P_1->pos
);
                 	shm_arbiter_buffer_drop(BUFFER_P0, 1);

}
             else
             {
                 right_processed += 1;
                 stream_args_P_1->pos
= process(rn, rp, stream_args_P_1->pos
, stream_args_P_0->pos
);
                 	shm_arbiter_buffer_drop(BUFFER_P1, 1);

}
         
                
                return 1;
            }
            
                    
                }
                    
                }
            
            bg_update(&BG_Ps, Ps_ORDER_EXP);
            // does not specifies order, we take first n even sources
is_selection_successful = bg_get_first_n(&BG_Ps, 2, &chosen_streams);

            if (is_selection_successful) {
                
                {
                    shm_stream *F = chosen_streams[0]->stream;
shm_arbiter_buffer *BUFFER_F = chosen_streams[0]->buffer;
STREAM_Primes_ARGS *stream_args_F = (STREAM_Primes_ARGS *)chosen_streams[0]->args;
char* e1_F; size_t i1_F; char* e2_F; size_t i2_F;
int count_F = shm_arbiter_buffer_peek(BUFFER_F, 1, (void **)&e1_F, &i1_F, (void**)&e2_F, &i2_F);
shm_stream *S = chosen_streams[1]->stream;
shm_arbiter_buffer *BUFFER_S = chosen_streams[1]->buffer;
STREAM_Primes_ARGS *stream_args_S = (STREAM_Primes_ARGS *)chosen_streams[1]->args;

                    
            int TEMPARR2[] = {PRIMES_PRIME};

            
                if (are_events_in_head(e1_F, i1_F, e2_F, i2_F, 
                count_F, sizeof(STREAM_Primes_out), TEMPARR2, 1)) {
                    
            STREAM_Primes_out * event_for_n = (STREAM_Primes_out *) get_event_at_index(e1_F, i1_F, e2_F, i2_F, sizeof(STREAM_Primes_out), 0);
int n = event_for_n->cases.Prime.n;

STREAM_Primes_out * event_for_p = (STREAM_Primes_out *) get_event_at_index(e1_F, i1_F, e2_F, i2_F, sizeof(STREAM_Primes_out), 0);
int p = event_for_p->cases.Prime.p;


            if(count < 10 || stream_args_F->pos<= stream_args_S->pos) {
                
                stream_args_F->pos
= process(n, p, stream_args_F->pos
, stream_args_S->pos
);
                 if (F == EV_SOURCE_P_0) {
                     left_processed ++;
                 } else {
                     right_processed ++;
                 } 
             
                	shm_arbiter_buffer_drop(BUFFER_F, 1);

                return 1;
            }
            
                    
                }
            
            int TEMPARR3[] = {PRIMES_HOLE};

            
                if (are_events_in_head(e1_F, i1_F, e2_F, i2_F, 
                count_F, sizeof(STREAM_Primes_out), TEMPARR3, 1)) {
                    
            STREAM_Primes_out * event_for_n = (STREAM_Primes_out *) get_event_at_index(e1_F, i1_F, e2_F, i2_F, sizeof(STREAM_Primes_out), 0);
int n = event_for_n->cases.hole.n;


            if(true ) {
                
                if(
stream_args_F->pos< 
stream_args_S->pos)
                 {
                     int removed = intmap_remove_upto(&buf, 
stream_args_F->pos+n);
                     count -= removed;
                 }
                 
stream_args_F->pos+= n;
                 if (F == EV_SOURCE_P_0) {
                     left_processed += n;
                     left_holes += 1;
                     left_drops += n;
                 } else {
                     right_processed += n;
                     right_holes += 1;
                     right_drops += n;
                 }
             
                	shm_arbiter_buffer_drop(BUFFER_F, 1);

                return 1;
            }
            
                    
                }
            
                }
                
                {
                    shm_stream *F = chosen_streams[1]->stream;
shm_arbiter_buffer *BUFFER_F = chosen_streams[1]->buffer;
STREAM_Primes_ARGS *stream_args_F = (STREAM_Primes_ARGS *)chosen_streams[1]->args;
char* e1_F; size_t i1_F; char* e2_F; size_t i2_F;
int count_F = shm_arbiter_buffer_peek(BUFFER_F, 1, (void **)&e1_F, &i1_F, (void**)&e2_F, &i2_F);
shm_stream *S = chosen_streams[0]->stream;
shm_arbiter_buffer *BUFFER_S = chosen_streams[0]->buffer;
STREAM_Primes_ARGS *stream_args_S = (STREAM_Primes_ARGS *)chosen_streams[0]->args;

                    
            int TEMPARR2[] = {PRIMES_PRIME};

            
                if (are_events_in_head(e1_F, i1_F, e2_F, i2_F, 
                count_F, sizeof(STREAM_Primes_out), TEMPARR2, 1)) {
                    
            STREAM_Primes_out * event_for_n = (STREAM_Primes_out *) get_event_at_index(e1_F, i1_F, e2_F, i2_F, sizeof(STREAM_Primes_out), 0);
int n = event_for_n->cases.Prime.n;

STREAM_Primes_out * event_for_p = (STREAM_Primes_out *) get_event_at_index(e1_F, i1_F, e2_F, i2_F, sizeof(STREAM_Primes_out), 0);
int p = event_for_p->cases.Prime.p;


            if(count < 10 || stream_args_F->pos<= stream_args_S->pos) {
                
                stream_args_F->pos
= process(n, p, stream_args_F->pos
, stream_args_S->pos
);
                 if (F == EV_SOURCE_P_0) {
                     left_processed ++;
                 } else {
                     right_processed ++;
                 } 
             
                	shm_arbiter_buffer_drop(BUFFER_F, 1);

                return 1;
            }
            
                    
                }
            
            int TEMPARR3[] = {PRIMES_HOLE};

            
                if (are_events_in_head(e1_F, i1_F, e2_F, i2_F, 
                count_F, sizeof(STREAM_Primes_out), TEMPARR3, 1)) {
                    
            STREAM_Primes_out * event_for_n = (STREAM_Primes_out *) get_event_at_index(e1_F, i1_F, e2_F, i2_F, sizeof(STREAM_Primes_out), 0);
int n = event_for_n->cases.hole.n;


            if(true ) {
                
                if(
stream_args_F->pos< 
stream_args_S->pos)
                 {
                     int removed = intmap_remove_upto(&buf, 
stream_args_F->pos+n);
                     count -= removed;
                 }
                 
stream_args_F->pos+= n;
                 if (F == EV_SOURCE_P_0) {
                     left_processed += n;
                     left_holes += 1;
                     left_drops += n;
                 } else {
                     right_processed += n;
                     right_holes += 1;
                     right_drops += n;
                 }
             
                	shm_arbiter_buffer_drop(BUFFER_F, 1);

                return 1;
            }
            
                    
                }
            
                }
                
            }
            _Bool ok = 1;
if (count_P0 >= 1) {if (count_P1 >= 1) {	ok = 0;
}}if (ok == 0) {
	fprintf(stderr, "Prefix of 'P0':\n");
	count_P0 = shm_arbiter_buffer_peek(BUFFER_P0, 5, &e1_P0, &i1_P0, &e2_P0, &i2_P0);
	print_buffer_prefix(BUFFER_P0, i1_P0 + i2_P0, count_P0, e1_P0, i1_P0, e2_P0, i2_P0);
	fprintf(stderr, "Prefix of 'P1':\n");
	count_P1 = shm_arbiter_buffer_peek(BUFFER_P1, 5, &e1_P1, &i1_P1, &e2_P1, &i2_P1);
	print_buffer_prefix(BUFFER_P1, i1_P1 + i2_P1, count_P1, e1_P1, i1_P1, e2_P1, i2_P1);
fprintf(stderr, "No rule matched even though there was enough events, NO PROGRESS!\n");assert(0);}
if (++RULE_SET_rs_nomatch_cnt > 8000000) {        	RULE_SET_rs_nomatch_cnt = 0;	fprintf(stderr, "\033[31mRule set 'rs' cycles long time without progress\033[0m\n");	fprintf(stderr, "Prefix of 'P0':\n");
	count_P0 = shm_arbiter_buffer_peek(BUFFER_P0, 5, &e1_P0, &i1_P0, &e2_P0, &i2_P0);
	print_buffer_prefix(BUFFER_P0, i1_P0 + i2_P0, count_P0, e1_P0, i1_P0, e2_P0, i2_P0);
	fprintf(stderr, "Prefix of 'P1':\n");
	count_P1 = shm_arbiter_buffer_peek(BUFFER_P1, 5, &e1_P1, &i1_P1, &e2_P1, &i2_P1);
	print_buffer_prefix(BUFFER_P1, i1_P1 + i2_P1, count_P1, e1_P1, i1_P1, e2_P1, i2_P1);
fprintf(stderr, "Seems all rules are waiting for some events that are not coming\n");}
	return 0;
}
int arbiter() {
    
    while (!are_streams_done()) {
        int rule_sets_match_count = 0;
		if (current_rule_set == SWITCH_TO_RULE_SET_rs) { 
			rule_sets_match_count += RULE_SET_rs();
		}

        if(rule_sets_match_count == 0) {
            // increment counter of no consecutive matches
            no_matches_count++;
        } else {
            // if there is a match reinit counter
            no_matches_count = 0;
        }
        
        if(no_matches_count == no_consecutive_matches_limit) {
            printf("******** NO RULES MATCHED FOR %d ITERATIONS, exiting program... **************\n", no_consecutive_matches_limit);
            print_buffers_state();
            // cleanup code
            destroy_intmap(&buf);
 int left_skipped = left_processed - compared - left_drops;
 int right_skipped = left_processed - compared - right_drops;
 printf("LEFT: processed %d events (%d compared, %d dropped (in %d holes), %d skipped) \n", left_processed, compared, left_drops, left_holes, left_skipped);
 
 printf("RIGHT: processed %d events (%d compared, %d dropped (in %d holes), %d skipped) \n", right_processed, compared, right_drops, right_holes, right_skipped);
 
 
            abort();
        }
    }
    shm_monitor_set_finished(monitor_buffer);
}
    

static void sig_handler(int sig) {
	printf("signal %d caught...", sig);	shm_stream_detach(EV_SOURCE_P_0);
	shm_stream_detach(EV_SOURCE_P_1);
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

	chosen_streams = (dll_node *) malloc(2); // the maximum size this can have is the total number of event sources
	arbiter_counter = malloc(sizeof(int));
	*arbiter_counter = 10;
	init_intmap(&buf);
 
stream_args_P_0 = malloc(sizeof(STREAM_Primes_ARGS));
stream_args_P_1 = malloc(sizeof(STREAM_Primes_ARGS));
	stream_args_P_0->pos = 0;
	stream_args_P_1->pos = 0;


	// connect to event source P_0
	EV_SOURCE_P_0 = shm_stream_create_from_argv("P_0", argc, argv);
	BUFFER_P0 = shm_arbiter_buffer_create(EV_SOURCE_P_0,  sizeof(STREAM_Primes_out), 1024);

	// register events in P_0
	if (shm_stream_register_event(EV_SOURCE_P_0, "Prime", PRIMES_PRIME) < 0) {
		fprintf(stderr, "Failed registering event Prime for stream P_0 : Primes\n");
		fprintf(stderr, "Available events:\n");
		shm_stream_dump_events(EV_SOURCE_P_0);
		abort();
	}
	// connect to event source P_1
	EV_SOURCE_P_1 = shm_stream_create_from_argv("P_1", argc, argv);
	BUFFER_P1 = shm_arbiter_buffer_create(EV_SOURCE_P_1,  sizeof(STREAM_Primes_out), 1024);

	// register events in P_1
	if (shm_stream_register_event(EV_SOURCE_P_1, "Prime", PRIMES_PRIME) < 0) {
		fprintf(stderr, "Failed registering event Prime for stream P_1 : Primes\n");
		fprintf(stderr, "Available events:\n");
		shm_stream_dump_events(EV_SOURCE_P_1);
		abort();
	}

     // activate buffers
	shm_arbiter_buffer_set_active(BUFFER_P0, true);
	shm_arbiter_buffer_set_active(BUFFER_P1, true);

 	monitor_buffer = shm_monitor_buffer_create(sizeof(STREAM_NumberPairs_out), 4);
 	
 		// init buffer groups
	init_buffer_group(&BG_Ps);
	bg_insert(&BG_Ps, EV_SOURCE_P_0, BUFFER_P0,stream_args_P_0,Ps_ORDER_EXP);
	bg_insert(&BG_Ps, EV_SOURCE_P_1, BUFFER_P1,stream_args_P_1,Ps_ORDER_EXP);
        

 	
     // create source-events threads
	thrd_create(&THREAD_P_0, PERF_LAYER_P,BUFFER_P0);
	thrd_create(&THREAD_P_1, PERF_LAYER_P,BUFFER_P1);


     // create arbiter thread
     thrd_create(&ARBITER_THREAD, arbiter, 0);
     
 
    // monitor
    STREAM_NumberPairs_out * received_event;
    while(true) {
        received_event = fetch_arbiter_stream(monitor_buffer);
        if (received_event == NULL) {
            break;
        }

		if (received_event->head.kind == 2) {
			int i = received_event->cases.NumberPair.i;
			int n = received_event->cases.NumberPair.n;
			int m = received_event->cases.NumberPair.m;

		  if (true ) {
		      if(n!=m)
         {
           printf("ERROR at index %i: %i is not equal to %i\n", i, n, m);
         }
     
		  }
		}
        
        shm_monitor_buffer_consume(monitor_buffer, 1);
    }
    
 	
	destroy_buffer_group(&BG_Ps);
	shm_stream_destroy(EV_SOURCE_P_0);
	shm_stream_destroy(EV_SOURCE_P_1);
	shm_arbiter_buffer_free(BUFFER_P0);
	shm_arbiter_buffer_free(BUFFER_P1);
	free(arbiter_counter);
	free(monitor_buffer);
	free(chosen_streams);
	free(stream_args_P_0);
	free(stream_args_P_1);

	
destroy_intmap(&buf);
 int left_skipped = left_processed - compared - left_drops;
 int right_skipped = left_processed - compared - right_drops;
 printf("LEFT: processed %d events (%d compared, %d dropped (in %d holes), %d skipped) \n", left_processed, compared, left_drops, left_holes, left_skipped);
 
 printf("RIGHT: processed %d events (%d compared, %d dropped (in %d holes), %d skipped) \n", right_processed, compared, right_drops, right_holes, right_skipped);
 
 
}
