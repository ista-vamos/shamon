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
// event declarations for stream type Events
struct _EVENT_E {
	long n;

};
typedef struct _EVENT_E EVENT_E;

// input stream for stream type Events
struct _STREAM_Events_in {
    shm_event head;
    union {
        EVENT_E E;
    }cases;
};
typedef struct _STREAM_Events_in STREAM_Events_in;

// output stream for stream type Events
struct _STREAM_Events_out {
    shm_event head;
    union {
        EVENT_hole hole;
        EVENT_E E;
    }cases;
};
typedef struct _STREAM_Events_out STREAM_Events_out;
        

enum Events_kinds {
EVENTS_E = 2,
EVENTS_HOLE = 1,
};


int *arbiter_counter;
// monitor buffer
shm_monitor_buffer *monitor_buffer;

bool is_selection_successful;
dll_node **chosen_streams; // used in rule set for get_first/last_n

// globals code
STREAM_Events_out *arbiter_outevent;

size_t processed = 0;
 size_t dropped = 0;
 size_t holes_num = 0;
 
bool SHOULD_KEEP_Src(shm_stream * s, shm_event * e) {
    return true;
}
            

atomic_int count_event_streams = 1;

// declare event streams
shm_stream *EV_SOURCE_Src;


// event sources threads
thrd_t THREAD_Src;


// declare arbiter thread
thrd_t ARBITER_THREAD;
const int SWITCH_TO_RULE_SET_rules = 0;

static size_t RULE_SET_rules_nomatch_cnt = 0;

int current_rule_set = SWITCH_TO_RULE_SET_rules;

// Arbiter buffer for event source Src
shm_arbiter_buffer *BUFFER_Src;




// buffer groups



int PERF_LAYER_Src (shm_arbiter_buffer *buffer) {
    shm_stream *stream = shm_arbiter_buffer_stream(buffer);   
    STREAM_Events_in *inevent;
    STREAM_Events_out *outevent;   

    // wait for active buffer
    while ((!shm_arbiter_buffer_active(buffer))){
        sleep_ns(10);
    }
    while(true) {
        inevent = stream_filter_fetch(stream, buffer, &SHOULD_KEEP_Src);
        
        if (inevent == NULL) {
            // no more events
            break;
        }
        outevent = shm_arbiter_buffer_write_ptr(buffer);
        
        memcpy(outevent, inevent, sizeof(STREAM_Events_out));
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
	c += are_there_events(BUFFER_Src);

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
STREAM_Events_out *arbiter_outevent;
int RULE_SET_rules();



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
            printf("E\n");
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
	event_index = get_event_at_head(BUFFER_Src);
	printf("Src -> ");
	print_event_name(0, event_index);

}    
    

int RULE_SET_rules() {
char* e1_Src; size_t i1_Src; char* e2_Src; size_t i2_Src;
int count_Src = shm_arbiter_buffer_peek(BUFFER_Src, 1, (void**)&e1_Src, &i1_Src, (void**)&e2_Src, &i2_Src);

            int TEMPARR0[] = {EVENTS_E};

            
                if (are_events_in_head(e1_Src, i1_Src, e2_Src, i2_Src, 
                count_Src, sizeof(STREAM_Events_out), TEMPARR0, 1)) {
                    
            STREAM_Events_out * event_for_n = (STREAM_Events_out *) get_event_at_index(e1_Src, i1_Src, e2_Src, i2_Src, sizeof(STREAM_Events_out), 0);
long n = event_for_n->cases.E.n;


            if(true ) {
                
                
        arbiter_outevent = (STREAM_Events_out *)shm_monitor_buffer_write_ptr(monitor_buffer);
         arbiter_outevent->head.kind = 2;
    arbiter_outevent->head.id = (*arbiter_counter)++;
    ((STREAM_Events_out *) arbiter_outevent)->cases.E.n = n;

         shm_monitor_buffer_write_finish(monitor_buffer);
        
                	shm_arbiter_buffer_drop(BUFFER_Src, 1);

                return 1;
            }
            
                    
                }
            
            int TEMPARR1[] = {EVENTS_HOLE};

            
                if (are_events_in_head(e1_Src, i1_Src, e2_Src, i2_Src, 
                count_Src, sizeof(STREAM_Events_out), TEMPARR1, 1)) {
                    
            STREAM_Events_out * event_for_n = (STREAM_Events_out *) get_event_at_index(e1_Src, i1_Src, e2_Src, i2_Src, sizeof(STREAM_Events_out), 0);
int n = event_for_n->cases.hole.n;


            if(true ) {
                
                
        arbiter_outevent = (STREAM_Events_out *)shm_monitor_buffer_write_ptr(monitor_buffer);
         arbiter_outevent->head.kind = 1;
    arbiter_outevent->head.id = (*arbiter_counter)++;
    ((STREAM_Events_out *) arbiter_outevent)->cases.hole.n = n;

         shm_monitor_buffer_write_finish(monitor_buffer);
        
                	shm_arbiter_buffer_drop(BUFFER_Src, 1);

                return 1;
            }
            
                    
                }
            _Bool ok = 1;
if (count_Src >= 1) {	ok = 0;
}if (ok == 0) {
	fprintf(stderr, "Prefix of 'Src':\n");
	count_Src = shm_arbiter_buffer_peek(BUFFER_Src, 5, &e1_Src, &i1_Src, &e2_Src, &i2_Src);
	print_buffer_prefix(BUFFER_Src, i1_Src + i2_Src, count_Src, e1_Src, i1_Src, e2_Src, i2_Src);
fprintf(stderr, "No rule matched even though there was enough events, NO PROGRESS!\n");assert(0);}
if (++RULE_SET_rules_nomatch_cnt > 8000000) {        	RULE_SET_rules_nomatch_cnt = 0;	fprintf(stderr, "\033[31mRule set 'rules' cycles long time without progress\033[0m\n");	fprintf(stderr, "Prefix of 'Src':\n");
	count_Src = shm_arbiter_buffer_peek(BUFFER_Src, 5, &e1_Src, &i1_Src, &e2_Src, &i2_Src);
	print_buffer_prefix(BUFFER_Src, i1_Src + i2_Src, count_Src, e1_Src, i1_Src, e2_Src, i2_Src);
fprintf(stderr, "Seems all rules are waiting for some events that are not coming\n");}
	return 0;
}
int arbiter() {
    
    while (!are_streams_done()) {
        int rule_sets_match_count = 0;
		if (current_rule_set == SWITCH_TO_RULE_SET_rules) { 
			rule_sets_match_count += RULE_SET_rules();
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
            printf("Processed %lu events, dropped %lu events in %lu holes\n",
         processed, dropped, holes_num);
 
            abort();
        }
    }
    shm_monitor_set_finished(monitor_buffer);
}
    

static void sig_handler(int sig) {
	printf("signal %d caught...", sig);	shm_stream_detach(EV_SOURCE_Src);
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

	chosen_streams = (dll_node *) malloc(1); // the maximum size this can have is the total number of event sources
	arbiter_counter = malloc(sizeof(int));
	*arbiter_counter = 10;
	


	// connect to event source Src
	EV_SOURCE_Src = shm_stream_create_from_argv("Src", argc, argv);
	BUFFER_Src = shm_arbiter_buffer_create(EV_SOURCE_Src,  sizeof(STREAM_Events_out), 2048);

	// register events in Src
	if (shm_stream_register_event(EV_SOURCE_Src, "E", EVENTS_E) < 0) {
		fprintf(stderr, "Failed registering event E for stream Src : Events\n");
		fprintf(stderr, "Available events:\n");
		shm_stream_dump_events(EV_SOURCE_Src);
		abort();
	}

     // activate buffers
	shm_arbiter_buffer_set_active(BUFFER_Src, true);

 	monitor_buffer = shm_monitor_buffer_create(sizeof(STREAM_Events_out), 4);
 	
 		// init buffer groups
	
 	
     // create source-events threads
	thrd_create(&THREAD_Src, PERF_LAYER_Src,BUFFER_Src);


     // create arbiter thread
     thrd_create(&ARBITER_THREAD, arbiter, 0);
     
 
    // monitor
    STREAM_Events_out * received_event;
    while(true) {
        received_event = fetch_arbiter_stream(monitor_buffer);
        if (received_event == NULL) {
            break;
        }

		if (received_event->head.kind == 2) {
			long n = received_event->cases.E.n;

		  if (true ) {
		      ++processed;
     
		  }
		}
        
		if (received_event->head.kind == 1) {
			int n = received_event->cases.hole.n;

		  if (true ) {
		      ++holes_num;
       dropped += n;
     
		  }
		}
        
        shm_monitor_buffer_consume(monitor_buffer, 1);
    }
    
 	
	shm_stream_destroy(EV_SOURCE_Src);
	shm_arbiter_buffer_free(BUFFER_Src);
	free(arbiter_counter);
	free(monitor_buffer);
	free(chosen_streams);

	
printf("Processed %lu events, dropped %lu events in %lu holes\n",
         processed, dropped, holes_num);
 
}
