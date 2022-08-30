#include "shamon.h"
#include "mmlib.h"
#include "monitor.h"
#include "compiler_utils.h"
#include <threads.h>
#include <stdio.h>

struct _EVENT_hole
{
  uint64_t n;
};
typedef struct _EVENT_hole EVENT_hole;

// globals code

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
	int pos;

};
typedef struct _STREAM_Primes_in STREAM_Primes_in;

// output stream for stream type Primes
struct _STREAM_Primes_out {
    shm_event head;
    union {
        EVENT_hole hole;
        EVENT_Prime Prime;
    }cases;
	int pos;

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

// Arbiter buffer for event source P (0)
shm_arbiter_buffer *BUFFER_P0;

// Arbiter buffer for event source P (1)
shm_arbiter_buffer *BUFFER_P1;


// monitor buffer
shm_monitor_buffer *monitor_buffer;

// buffer groups

bool Ps_ORDER_EXP (shm_stream *ev1, shm_stream *ev2) {
    return false;
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


bool exists_open_streams() {
    return count_event_streams > 0;
}
    
bool check_n_events(shm_arbiter_buffer* b, size_t n) {
    // checks if there are exactly n elements on a given stream s
    void* e1; size_t i1;
	void* e2; size_t i2;
	return shm_arbiter_buffer_peek(b,0, &e1, &i1, &e2, &i2) == n;
}


bool are_events_in_head(shm_arbiter_buffer *b, size_t ev_size, int event_kinds[], int n_events) {
    char* e1; size_t i1;
	char* e2; size_t i2;
	int count = shm_arbiter_buffer_peek(b, n_events, (void **)&e1, &i1,(void**) &e2, &i2);
	if (count < n_events) {
	    return false;
    }
    
    
	int i = 0;
	while (i < i1) {
	    shm_event * ev = (shm_event *) (e1);
	     if (ev->kind != event_kinds[i]) {
	        return false;
	    }
	    i+=1;
	    e1 += ev_size;
	}

	i = 0;
	while (i < i2) {
	    shm_event * ev = (shm_event *) e2;
	     if (ev->kind != event_kinds[i1+i]) {
	        return false;
	    }
	    i+=1;
	    e2 += ev_size;
	}

	return true;
}

shm_event * get_event_at_index(char* e1, size_t i1, char* e2, size_t i2, size_t size_event, int element_index) {
	if (element_index < i1) {
		return (shm_event *) (e1 + (element_index*size_event));
	} else {
		element_index -=i1;
		return (shm_event *) (e2 + (element_index*size_event));
	}
}
int RULE_SET_rs(int *);


int RULE_SET_rs(int *arbiter_counter) {
shm_stream *chosen_streams; // used for match fun
            int TEMPARR0[] = {2};
int TEMPARR1[] = {2};

            if (are_events_in_head(BUFFER_P_0, sizeof(STREAM_Primes_out), TEMPARR1, 1)) {
                    if (are_events_in_head(BUFFER_P_1, sizeof(STREAM_Primes_out), TEMPARR0, 1)) {
                    
            if(true ) {
                
                char* e1_P; size_t i1_P;
	            char* e2_P; size_t i2_P;
	            shm_arbiter_buffer_peek(BUFFER_P, 1, (void**)&e1_P, &i1_P,(void**) &e2_P, &i2_P);
            
                STREAM_Primes_out * event_for_ln = (STREAM_Primes_out *) get_event_at_index(e1_P, i1_P, e2_P, i2_P, sizeof(STREAM_Primes_out), 0);
int ln = event_for_ln->cases.Prime.n;

STREAM_Primes_out * event_for_lp = (STREAM_Primes_out *) get_event_at_index(e1_P, i1_P, e2_P, i2_P, sizeof(STREAM_Primes_out), 0);
int lp = event_for_lp->cases.Prime.p;

STREAM_Primes_out * event_for_rn = (STREAM_Primes_out *) get_event_at_index(e1_P, i1_P, e2_P, i2_P, sizeof(STREAM_Primes_out), 0);
int rn = event_for_rn->cases.Prime.n;

STREAM_Primes_out * event_for_rp = (STREAM_Primes_out *) get_event_at_index(e1_P, i1_P, e2_P, i2_P, sizeof(STREAM_Primes_out), 0);
int rp = event_for_rp->cases.Prime.p;


                if(ln == rn)
             {
                 

        arbiter_outevent = shm_monitor_buffer_write_ptr(monitor_buffer);
         arbiter_outevent->head.kind = 2;
    arbiter_outevent->head.id = (*arbiter_counter)++;
    ((STREAM_NumberPairs_out *) arbiter_outevent)->cases.NumberPair.i = ln;
((STREAM_NumberPairs_out *) arbiter_outevent)->cases.NumberPair.n = lp;
((STREAM_NumberPairs_out *) arbiter_outevent)->cases.NumberPair.m = rp;

         shm_monitor_buffer_write_finish(monitor_buffer);
        	shm_arbiter_buffer_drop(BUFFER_P_0, 1);
	shm_arbiter_buffer_drop(BUFFER_P_1, 1);

intmap_clear(&buf);
             }
             else if(P_0->pos
<P_1->pos
)
             {
                 P_0->pos
= process(ln, lp, P_0->pos
, P_1->pos
);
                 	shm_arbiter_buffer_drop(BUFFER_P_0, 1);

}
             else
             {
                 P_1->pos
= process(rn, rp, posns[1], P_0->pos
);
                 	shm_arbiter_buffer_drop(BUFFER_P_1, 1);

}
         
            }
            
                }
                }
            
            int TEMPARR2[] = {1};

            
        
            chosen_streams = bg_get_first_n(&BG_Ps, 2);
            if (chosen_streams != NULL) {
                chosen_streams--;
shm_stream *F = chosen_streams;
chosen_streams--;
shm_stream *S = chosen_streams;

                if (are_events_in_head(BUFFER_F, sizeof(STREAM_Primes_out), TEMPARR2, 1)) {
                    
            if(true ) {
                
                char* e1_F; size_t i1_F;
	            char* e2_F; size_t i2_F;
	            shm_arbiter_buffer_peek(BUFFER_F, 1, (void**)&e1_F, &i1_F,(void**) &e2_F, &i2_F);
            
                STREAM_Primes_out * event_for_n = (STREAM_Primes_out *) get_event_at_index(e1_F, i1_F, e2_F, i2_F, sizeof(STREAM_Primes_out), 0);
int n = event_for_n->cases.hole.n;


                if(
F->pos< 
S->pos)
             {
                 count -= intmap_remove_upto(&buf, 
F->pos+n);
             }
             
F->pos+= n;
         
            }
            
                }
            }
                    
            
            int TEMPARR3[] = {2};

            
        
            chosen_streams = bg_get_first_n(&BG_Ps, 2);
            if (chosen_streams != NULL) {
                chosen_streams--;
shm_stream *F = chosen_streams;
chosen_streams--;
shm_stream *S = chosen_streams;

                if (are_events_in_head(BUFFER_F, sizeof(STREAM_Primes_out), TEMPARR3, 1)) {
                    
            if(true ) {
                
                char* e1_F; size_t i1_F;
	            char* e2_F; size_t i2_F;
	            shm_arbiter_buffer_peek(BUFFER_F, 1, (void**)&e1_F, &i1_F,(void**) &e2_F, &i2_F);
            
                STREAM_Primes_out * event_for_n = (STREAM_Primes_out *) get_event_at_index(e1_F, i1_F, e2_F, i2_F, sizeof(STREAM_Primes_out), 0);
int n = event_for_n->cases.Prime.n;

STREAM_Primes_out * event_for_p = (STREAM_Primes_out *) get_event_at_index(e1_F, i1_F, e2_F, i2_F, sizeof(STREAM_Primes_out), 0);
int p = event_for_p->cases.Prime.p;


                F->pos
= process(n, p, F->pos
, S->pos
);
         
            }
            
                }
            }
                    
            }
int arbiter() {
    int arbiter_counter = 10;
    while (exists_open_streams()) {
    	RULE_SET_rs(&arbiter_counter);
	}
    shm_monitor_set_finished(monitor_buffer);
}
    
int main(int argc, char **argv) {
	initialize_events(); // Always call this first
	
	// init buffer groups
	init_buffer_group(&BG_Ps);
	bg_insert(BG_Ps, EV_SOURCE_P_0, Ps_ORDER_EXP);
	bg_insert(BG_Ps, EV_SOURCE_P_1, Ps_ORDER_EXP);
        

	// connect to event source P_0
	EV_SOURCE_P_0 = shm_stream_create("P_0", argc, argv);
	BUFFER_P_0 = shm_arbiter_buffer_create(EV_SOURCE_P_0,  sizeof(STREAM_Primes_out), 8);

	// connect to event source P_1
	EV_SOURCE_P_1 = shm_stream_create("P_1", argc, argv);
	BUFFER_P_1 = shm_arbiter_buffer_create(EV_SOURCE_P_1,  sizeof(STREAM_Primes_out), 8);


     // activate buffers
	shm_arbiter_buffer_set_active(BUFFER_P0, true);
	shm_arbiter_buffer_set_active(BUFFER_P1, true);

 	monitor_buffer = shm_monitor_buffer_create(sizeof(STREAM_NumberPairs_out), 64);
 	
     // create source-events threads
	thrd_create(&THREAD_P_0, PERF_LAYER_P,0);
	thrd_create(&THREAD_P_1, PERF_LAYER_P,0);


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
           printf("Error at index %i: %i is not equal to %i\n", i, n, m);
         }
     
		  }
		}
        
        shm_monitor_buffer_consume(monitor_buffer, 1);
    }
    
 	
	destroy_buffer_group(&BG_Ps);

	// destroy event sources
	shm_stream_destroy(EV_SOURCE_P_0);
	shm_stream_destroy(EV_SOURCE_P_1);


	// destroy arbiter buffers
	shm_arbiter_buffer_free(BUFFER_P0);
	shm_arbiter_buffer_free(BUFFER_P1);

	
	
}
