#define __mm_consume(B, k) shm_arbiter_buffer_drop((B), (k))
#define __mm_stream_peek(S, k, ptr1, len1, ptr2, len2)\
    shm_arbiter_buffer_peek((S), (k), (ptr1), (ptr2), (len2))
/* TODO evsize is not there right now */
#define __MM_CHECK_EVENT_KIND(ptr1, len1, ptr2, len2, n, evsize, kind)\
    ((n <= len1) ?\
        (shm_event_kind(ptr1 + n*evsize) == (kind)) :\
        (((n - len1) <= len2) ?\
            (shm_event_kind(ptr2 + (n-len1)*evsize) == (kind)) : 0))
