CPPFLAGS= -Wno-unused-result -I.. -I../../../DynamoRIO-Linux-9.0.0/include -I../../../DynamoRIO-Linux-9.0.0/ext/include
CFLAGS=-O3

all: monitor bank monbank monbanknw monitornw smtmonitor ../interact inputs.txt inputs100.txt #gennums_hardcoded-instr

inputs.txt : inputs.py
	python $^ 100000 > $@

inputs100.txt : inputs.py
	python $^ 100 > $@

../interact: ../interact.c
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -g

monitor: monitor.o shm_monitor.o shm_common.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -lrt -lm -lpthread

smtmonitor: smtmonitor.o shm_monitor.o shm_common.o smt.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -lrt -lm -lpthread -lz3

monitornw: monitornw.o shm_monitor.o shm_common.o
	$(CC) $^ $(CFLAGS) $(CPPFLAGS) -o $@ -lrt -lm -lpthread

monitor.o: monitor.c
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

smt.o: smt.c smt.h
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

smtmonitor.o: smtmonitor.c smt.h
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
	-rm *.o monitor bank monbank monbanknw monitornw ../interact inputs.txt inputs100.txt

.PHONY: clean all
