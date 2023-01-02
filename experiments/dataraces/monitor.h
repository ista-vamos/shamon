#include <inttypes.h>
#include <stdlib.h>

void monitor_handle_read(int tid, uint64_t timestamp, intptr_t addr);
void monitor_handle_read_many(int tid, uint64_t timestamp, intptr_t addr, size_t bytes);

void monitor_handle_write(int tid, uint64_t timestamp, intptr_t addr);
void monitor_handle_write_many(int tid, uint64_t timestamp, intptr_t addr, size_t bytes);

void monitor_handle_lock(int tid, uint64_t timestamp, intptr_t addr);

void monitor_handle_unlock(int tid, uint64_t timestamp, intptr_t addr);

void monitor_handle_skip_start(int tid, uint64_t timestamp);
void monitor_handle_skip_end(int tid, uint64_t timestamp);
void monitor_handle_alloc(int tid, uint64_t timestamp, intptr_t addr, size_t size);

void monitor_handle_free(int tid, uint64_t timestamp, intptr_t addr);
void monitor_handle_fork(int tid, uint64_t timestamp, int otherthread);

void monitor_handle_join(int tid, uint64_t timestamp, int otherthread);
void monitor_handle_done(int tid, uint64_t timestamp);

uint64_t GetRaceCount();
