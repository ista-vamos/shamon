#include <cstdlib>
#include <cstdio>
#include <cinttypes>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>

#define ADDRCELLSIZE 8
#define THREADCELLSIZE 4

using namespace std;

enum class ActionType {
	ATRead, ATWrite, ATLock, ATUnlock, ATAlloc, ATFree, ATFork, ATJoin, ATSkipStart, ATSkipEnd, ATDone
};

typedef union {
	struct { intptr_t addr; } read;
	struct { intptr_t addr; } write;
	struct { intptr_t addr; } lock;
	struct { intptr_t addr; } unlock;
	struct { intptr_t addr; size_t size; } alloc;
	struct { intptr_t addr; } free;
	struct { int newthreadid; } fork;
	struct { int threadid; } join;
} Action;

class Cell {
	public:
	Cell * next;
	size_t refcount;
	uint64_t timestamp;
	int threadid;
	ActionType type;
	Action action;
};

Cell * head=new Cell();
Cell * tail=head;
size_t cellcount=0;

class Lockset
{
	public:
	unordered_set<intptr_t> addrs;
	unordered_set<int> threads;
	unordered_set<int> skipthreads;
	unordered_set<int> skippedthreads;
};

class Info {
	public:
	Info(int owner):owner(owner),pos(0),alock(0),lockset()
	{
		pos=tail;
		lockset.threads.insert(owner);
	}
	Info(Info &a) =delete;
	Info(const Info &a) =delete;
	int owner;
	Cell * pos;
	intptr_t alock;
	Lockset lockset;
};

unordered_map<intptr_t, Info> Writes;
unordered_map<intptr_t, map<int, Info>> Reads;
unordered_map<intptr_t, int> LockThreads;
map<int, set<intptr_t>> ThreadLocks;

inline void enqueue_sync_event(int thrd, ActionType tp, Action &a)
{
	tail->threadid=thrd;
	tail->action=a;
	tail->type=tp;
	tail->next=new Cell();
	tail=tail->next;
	cellcount++;
}

inline bool thread_holds_lock(int thrd, intptr_t lock)
{
	auto posn = LockThreads.find(lock);
	if(posn!=LockThreads.end())
	{
		return (*posn).second==thrd;
	}
	return false;
}

void apply_lockset_rules(Lockset &ls, Cell *pos1, Cell *pos2, int owner2)
{
	do
	{
		switch(pos1->type)
		{
			case ActionType::ATLock:
			{
				if(ls.addrs.find(pos1->action.lock.addr)!=ls.addrs.end()||!ls.skipthreads.empty()||!ls.skippedthreads.empty())
				{
					ls.threads.insert(pos1->threadid);
				}
			}
			break;
			case ActionType::ATUnlock:
			{
				if(ls.threads.find(pos1->threadid)!=ls.threads.end()||ls.skipthreads.find(pos1->threadid)!=ls.skipthreads.end()||ls.skippedthreads.find(pos1->threadid)!=ls.skippedthreads.end())
				{
					ls.addrs.insert(pos1->action.unlock.addr);
				}
			}
			break;
			case ActionType::ATAlloc:
			{

			}
			break;
			case ActionType::ATFree:
			{

			}
			break;
			case ActionType::ATFork:
			{
				if(ls.threads.find(pos1->threadid)!=ls.threads.end()||ls.skipthreads.find(pos1->threadid)!=ls.skipthreads.end()||ls.skippedthreads.find(pos1->threadid)!=ls.skippedthreads.end())
				{
					ls.threads.insert(pos1->action.fork.newthreadid);
				}
			}
			break;
			case ActionType::ATJoin:
			{
				if(ls.threads.find(pos1->action.join.threadid)!=ls.threads.end()||ls.skipthreads.find(pos1->action.join.threadid)!=ls.skipthreads.end()||ls.skippedthreads.find(pos1->action.join.threadid)!=ls.skippedthreads.end())
				{
					ls.threads.insert(pos1->threadid);
				}
			}
			break;
			case ActionType::ATDone:
			{
			}
			break;
			case ActionType::ATRead:
			{
				if((!ls.addrs.empty())&&ls.addrs.find(pos1->action.read.addr)==ls.addrs.end()&&ls.skipthreads.find(pos1->threadid)==ls.skipthreads.end()&&ls.skippedthreads.find(pos1->threadid)==ls.skippedthreads.end())
				{
					printf("Found data race: Thread %i read from %li without synchronization\n", pos1->threadid, pos1->action.read.addr);
				}
				ls.addrs.clear();
				if(ls.threads.size()!=1||(*ls.threads.begin())!=pos1->threadid)
				{
					ls.threads.clear();
					ls.threads.insert(pos1->threadid);
				}
				ls.skippedthreads.clear();
			}
			break;
			case ActionType::ATWrite:
			{
				if((!ls.addrs.empty())&&ls.addrs.find(pos1->action.write.addr)==ls.addrs.end()&&ls.skipthreads.find(pos1->threadid)==ls.skipthreads.end()&&ls.skippedthreads.find(pos1->threadid)==ls.skippedthreads.end())
				{
					printf("Found data race: Thread %i wrote to %li without synchronization\n", pos1->threadid, pos1->action.write.addr);
				}
				ls.addrs.clear();
				if(ls.threads.size()!=1||(*ls.threads.begin())!=pos1->threadid)
				{
					ls.threads.clear();
					ls.threads.insert(pos1->threadid);
				}
				ls.skippedthreads.clear();
			}
			break;
			case ActionType::ATSkipStart:
			{
				ls.skipthreads.insert(pos1->threadid);
			}
			break;
			case ActionType::ATSkipEnd:
			{
				ls.skipthreads.erase(pos1->threadid);
				ls.skippedthreads.insert(pos1->threadid);
			}
			break;
		}
		pos1=pos1->next;
	} while(pos1!=pos2);
}

void check_happens_before(Info &info1, Info &info2)
{
	// if(info1->xact&&info2->xact)
	// {
	// 	return;
	// }
	if((info1.owner!=info2.owner)&&(thread_holds_lock(info2.owner,info1.alock)))
	{
		apply_lockset_rules(info1.lockset, info1.pos, info2.pos, info2.owner);
		info2.alock=0;
		auto posn=ThreadLocks.find(info1.owner);
		if(posn!=ThreadLocks.end())
		{
			auto posn2=posn->second.begin();
			if(posn2!=posn->second.end())
			{
				info2.alock=*posn2;
			}
		}
	}
}

void cell_rc_inc(Cell* cell)
{
	cell->refcount++;
}
void cell_rc_dec(Cell* cell)
{
	cell->refcount--;
	if(cell->refcount<=0)
	{
		if(cellcount>10000&&head->refcount==0)
		{
			while(head->refcount==0&&head->next!=0)
			{
				Cell* last=head;
				head=head->next;
				cellcount--;
				free(last);
			}
		}
	}
}

extern "C" void monitor_handle_read(int tid, uint64_t timestamp, intptr_t addr)
{
	auto &addrmap = Reads[addr];
	auto entry = addrmap.find(tid);
	cell_rc_inc(tail);
	if(entry==addrmap.end())
	{
		entry=addrmap.insert_or_assign(tid,tid).first;
	}
	else
	{
		entry->second.alock=0;
		entry->second.owner=tid;
		entry->second.lockset.addrs.clear();
		entry->second.lockset.threads.clear();
		entry->second.lockset.threads.insert(tid);
		cell_rc_dec(entry->second.pos);
		entry->second.pos=tail;
	}
	auto wentry=Writes.find(addr);
	if(wentry!=Writes.end())
	{
		check_happens_before(wentry->second,entry->second);
	}
}
extern "C" void monitor_handle_read_many(int tid, uint64_t timestamp, intptr_t addr, size_t bytes)
{
	for(size_t i=0;i<bytes;i++)
	{
		monitor_handle_read(tid,timestamp,addr+i);
	}
}

extern "C" void monitor_handle_write(int tid, uint64_t timestamp, intptr_t addr)
{
	Info info(tid);
	cell_rc_inc(tail);
	auto rinfo = Reads.find(addr);
	if(rinfo!=Reads.end())
	{
		for(auto tlp=ThreadLocks.begin();tlp!=ThreadLocks.end();tlp++)
		{
			auto trinfo = rinfo->second.find(tlp->first);
			if(trinfo!=rinfo->second.end())
			{
				check_happens_before(trinfo->second, info);
			}
		}
	}
	auto winfo = Writes.find(addr);
	if(winfo!=Writes.end())
	{
		check_happens_before(winfo->second, info);
		cell_rc_dec(winfo->second.pos);
		winfo->second=std::move(info);
	}
	else
	{
		Writes.insert_or_assign(addr, tid);
	}
	if(rinfo!=Reads.end())
	{
		for(auto tlp=rinfo->second.begin();tlp!=rinfo->second.end();tlp++)
		{
			cell_rc_dec(tlp->second.pos);
		}
		rinfo->second.clear();
	}
	Reads.extract(addr);
}
extern "C" void monitor_handle_write_many(int tid, uint64_t timestamp, intptr_t addr, size_t bytes)
{
	for(size_t i=0;i<bytes;i++)
	{
		monitor_handle_write(tid,timestamp,addr+i);
	}
}

extern "C" void monitor_handle_lock(int tid, uint64_t timestamp, intptr_t addr)
{
	Action a;
	a.lock.addr=addr;
	enqueue_sync_event(tid, ActionType::ATLock, a);
}

extern "C" void monitor_handle_unlock(int tid, uint64_t timestamp, intptr_t addr)
{
	Action a;
	a.unlock.addr=addr;
	enqueue_sync_event(tid, ActionType::ATUnlock, a);
}

extern "C" void monitor_handle_skip_start(int tid, uint64_t timestamp)
{
	Action a;
	enqueue_sync_event(tid, ActionType::ATSkipStart, a);
}
extern "C" void monitor_handle_skip_end(int tid, uint64_t timestamp)
{
	Action a;
	enqueue_sync_event(tid, ActionType::ATSkipEnd, a);
}
extern "C" void monitor_handle_alloc(int tid, uint64_t timestamp, intptr_t addr, size_t size)
{
	Action a;
	a.alloc.addr=addr;
	a.alloc.size=size;
	enqueue_sync_event(tid, ActionType::ATAlloc, a);
}

extern "C" void monitor_handle_free(int tid, uint64_t timestamp, intptr_t addr)
{
	Action a;
	a.free.addr=addr;
	enqueue_sync_event(tid, ActionType::ATFree, a);
}
extern "C" void monitor_handle_fork(int tid, uint64_t timestamp, int otherthread)
{
	Action a;
	a.fork.newthreadid=otherthread;
	enqueue_sync_event(tid, ActionType::ATFork, a);
}

extern "C" void monitor_handle_join(int tid, uint64_t timestamp, int otherthread)
{
	Action a;
	a.join.threadid=otherthread;
	enqueue_sync_event(tid, ActionType::ATJoin, a);
}

extern "C" void monitor_handle_done(int tid, uint64_t timestamp)
{
	Action a;
	enqueue_sync_event(tid, ActionType::ATDone, a);
}
