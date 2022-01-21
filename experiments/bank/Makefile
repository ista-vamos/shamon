CPPFLAGS=#-I.. -I/home/fabian/DynamoRIO-Linux-9.0.0/include -I/home/fabian/DynamoRIO-Linux-9.0.0/ext/include
CFLAGS=-O0 -g

all: monitor bank monbank monbanknw monitornw #gennums_hardcoded-instr

monitor: monitor.o shm_monitor.o shm_common.o
	$(CC) $^ -O0 -g $(CPPFLAGS) -o $@ -lrt -lm -lpthread

monitornw: monitornw.o shm_monitor.o shm_common.o
	$(CC) $^ -O0 -g $(CPPFLAGS) -o $@ -lrt -lm -lpthread

monitor.o: monitor.c
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

monitornw.o: monitor.c
	$(CC) -c $< -D SHM_DOMONITOR_NOWAIT $(CFLAGS) $(CPPFLAGS) -o $@

shm_common.o : ../../fastbuf/shm_common.c ../../fastbuf/shm_common.h
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

shm_monitor.o : ../../fastbuf/shm_monitor.c ../../fastbuf/shm_monitor.h ../../fastbuf/shm_common.h
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

shm_monitored.o : ../../fastbuf/shm_monitored.c ../../fastbuf/shm_monitored.h ../../fastbuf/shm_common.h
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

bank.o : bank.c bank.h
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

client.o : client.c bank.h common.h
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

monclient.o : client.c bank.h common.h ../../fastbuf/shm_monitored.h ../../fastbuf/shm_common.h
	$(CC) -c $< -D SHM_DOMONITOR_WAIT $(CFLAGS) $(CPPFLAGS) -o $@

monclientnw.o : client.c bank.h common.h ../../fastbuf/shm_monitored.h ../../fastbuf/shm_common.h
	$(CC) -c $< -D SHM_DOMONITOR_NOWAIT $(CFLAGS) $(CPPFLAGS) -o $@

bank: client.o bank.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -lpthread

monbank: monclient.o bank.o shm_monitored.o shm_common.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -lpthread

monbanknw: monclientnw.o bank.o shm_monitored.o shm_common.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -lpthread

clean:
	-rm *.o monitor bank monbank monbanknw monitornw

.PHONY: clean all
