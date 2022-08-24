#include "shamon.h"
#include "mmlib.h"
#include "monitor.h"
#include <threads.h>
#include <stdio.h>

struct _EVENT_hole
{
  uint64_t n;
};
typedef struct _EVENT_hole EVENT_hole;

// globals code
STREAM_NumberPairs_out *arbiter_outevent;
#include "intmap.h"
 
     intmap buf;
 
     void process(n, p, pos, opos){
 
         if(pos < opos) {
             int oval=0;
             if(intmap_get(&buf, n, &oval)){
                 

        arbiter_outevent = shm_monitor_buffer_write_ptr(monitor_buffer);
         arbiter_outevent->head.kind = 2;
    arbiter_outevent->head.id = (*arbiter_counter)++;
    ((STREAM_NumberPairs_out *) arbiter_outevent)->cases.NumberPair.i = n;
((STREAM_NumberPairs_out *) arbiter_outevent)->cases.NumberPair.n = oval;
((STREAM_NumberPairs_out *) arbiter_outevent)->cases.NumberPair.m = p;

         shm_monitor_buffer_write_finish(monitor_buffer);
        }
             count -= intmap_remove_upto(&buf, n);
         } else {
             if(count<10) {
                 intmap_insert(&buf, n, p);
                 count++;
             }
         }
         return n+1;
     }
 

// args for stream type Primes
struct _Primes_ARGS {
	int pos;

}
typedef struct  _Primes_ARGS  Primes_ARGS;
            
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
shm_stream *EV_SOURCE_P;


// event sources threads
thrd_t THREAD_P;


// declare arbiter thread
thrd_t ARBITER_THREAD;

// Arbiter buffer for event source P (0)
shm_arbiter_buffer *BUFFER_P0;

// Arbiter buffer for event source P (1)
shm_arbiter_buffer *BUFFER_P1;


// monitor buffer
shm_monitor_buffer *monitor_buffer;

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



