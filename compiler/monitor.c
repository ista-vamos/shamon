#include "shamon.h"
#include "mmlib.h"
#include <threads.h>


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
        

bool SHOULD_KEEP_Left(shm_stream * s, shm_event * e) {
    if (e->kind == 2) {
        return true;
    }
        
    return false;
}
bool SHOULD_KEEP_Right(shm_stream * s, shm_event * e) {
    if (e->kind == 2) {
        return true;
    }
        
    return false;
}


int count_event_streams = 2;

// declare event streams
shm_stream *EV_SOURCE_Left;
shm_stream *EV_SOURCE_Right;

//declare flags for event streams
bool is_Left_done;
bool is_Right_done;

// event sources threads
thrd_t THREAD_Left;
thrd_t THREAD_Right;

// declare arbiter thread
thrd_t ARBITER_THREAD;
// Arbiter buffer for event source Left
shm_arbiter_buffer *BUFFER_Left;

// Arbiter buffer for event source Right
shm_arbiter_buffer *BUFFER_Right;


int PERF_LAYER_Left (void *arg) {
    shm_arbiter_buffer *buffer = BUFFER_Left;
    shm_stream *stream = shm_arbiter_buffer_stream(buffer);   

    STREAM_Primes_in *inevent;
    STREAM_Primes_out *outevent;   

    // wait for active buffer
    while ((!shm_arbiter_buffer_active(buffer))){
        sleep_ns(10);
    }
    
    is_Left_done = 0;
    while(true) {
        inevent = stream_filter_fetch(stream, buffer, &SHOULD_KEEP_Left);
        
        if (inevent == NULL) {
            // no more events
            is_Left_done = 1;
            break;
        }
        outevent = shm_arbiter_buffer_write_ptr(buffer);
        
        switch ((inevent->head).kind) {

			case 2:

				(outevent->head).kind = 2;
				(outevent->head).id = (inevent->head).id;
				outevent->cases.Prime.n = n;
				outevent->cases.Prime.p = p ;

				break;
            default:
                printf("Default case executed in thread for event source Left. Exiting thread...");
                return 1;
        }
        shm_arbiter_buffer_write_finish(b);
        shm_stream_consume(stream, 1);
    }     
}

int PERF_LAYER_Right (void *arg) {
    shm_arbiter_buffer *buffer = BUFFER_Right;
    shm_stream *stream = shm_arbiter_buffer_stream(buffer);   

    STREAM_Primes_in *inevent;
    STREAM_Primes_out *outevent;   

    // wait for active buffer
    while ((!shm_arbiter_buffer_active(buffer))){
        sleep_ns(10);
    }
    
    is_Right_done = 0;
    while(true) {
        inevent = stream_filter_fetch(stream, buffer, &SHOULD_KEEP_Right);
        
        if (inevent == NULL) {
            // no more events
            is_Right_done = 1;
            break;
        }
        outevent = shm_arbiter_buffer_write_ptr(buffer);
        
        switch ((inevent->head).kind) {

			case 2:

				(outevent->head).kind = 2;
				(outevent->head).id = (inevent->head).id;
				outevent->cases.Prime.n = n;
				outevent->cases.Prime.p = p ;

				break;
            default:
                printf("Default case executed in thread for event source Right. Exiting thread...");
                return 1;
        }
        shm_arbiter_buffer_write_finish(b);
        shm_stream_consume(stream, 1);
    }     
}

bool exists_open_streams() {
    int c = 0;
	c += is_Left_done;
	c += is_Right_done;
	return c < count_event_streams;
}
    
bool check_n_events(shm_stream* s, size_t n) {
    // checks if there are exactly n elements on a given stream s
    void* e1; size_t i1;
	void* e2; size_t i2;
	return shm_arbiter_buffer_peek(b,0, &e1, &i1, &e2, &i2) == n;
}

bool are_events_in_head(shm_stream *s, shm_arbiter_buffer *b, size_t ev_size, int event_kinds[], int n_events) {
    char* e1; size_t i1;
	char* e2; size_t i2;
	int count = shm_arbiter_buffer_peek(b, c_events, &e1, &i1, &e2, &i2);
	if (count < n_events) {
	    return false;
	}
	
	int i = 0;
	while (i1 > 0) {
	    i1--;
	    shm_event * ev = (shm_event *) (e1 + (i1*ev_size));
	     if (ev->head.kind != event_kinds[i]) {
	        return false;
	    }
	    i+=1;
	}
	
	while (i2 > 0) {
	    i2--;
	    shm_event * ev = (shm_event *) (e2 + (i2*ev_size));
	     if (ev->head.kind != event_kinds[i]) {
	        return false;
	    }
	    i+=1;
	}
	
	return true;
}

int RULE_SET_rs();

int RULE_SET_rs() {
    
    if (are_events_in_head(EV_SOURCE_Left, BUFFER_Left, sizeof(STREAM_Primes_out), [2], 1) && are_events_in_head(EV_SOURCE_Right, BUFFER_Right, sizeof(STREAM_Primes_out), [2], 1)) {
        if(true ) {
        }
    }
        
    if (are_events_in_head(EV_SOURCE_Left, BUFFER_Left, sizeof(STREAM_Primes_out), [1], 1)) {
        if(true ) {
        }
    }
        
    if (are_events_in_head(EV_SOURCE_Right, BUFFER_Right, sizeof(STREAM_Primes_out), [1], 1)) {
        if(true ) {
        }
    }
        
}

int arbiter() {

    while (exists_open_stream()) {
    	RULE_SET_rs();
	}
}
    
int main(int argc, char **argv) {
    initialize_events(); // Always call this first
    
	// connect to event source Left
	EV_SOURCE_Left = shm_stream_create("Left", argc, argv);
	BUFFER_Left = shm_arbiter_buffer_create(EV_SOURCE_Left,  sizeof(STREAM_Primes_out), 8);

	// connect to event source Right
	EV_SOURCE_Right = shm_stream_create("Right", argc, argv);
	BUFFER_Right = shm_arbiter_buffer_create(EV_SOURCE_Right,  sizeof(STREAM_Primes_out), 8);


    // activate buffers
	shm_arbiter_buffer_set_active(BUFFER_Left, true);
	shm_arbiter_buffer_set_active(BUFFER_Right, true);


    // create source-events threads
	thrd_create(&THREAD_Left, PERF_LAYER_Left);
	thrd_create(&THREAD_Right, PERF_LAYER_Right);


    // create arbiter thread
    thrd_create(&ARBITER_THREAD, arbiter);
     
    // destroy event sources
	shm_stream_destroy(EV_SOURCE_Left);
	shm_stream_destroy(EV_SOURCE_Right);

    // destroy arbiter buffers
	shm_arbiter_buffer_free(BUFFER_Left);
	shm_arbiter_buffer_free(BUFFER_Right);

}
