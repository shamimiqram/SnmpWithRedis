CC=gcc
CFLAGS=-Iinclude -I/usr/include/net-snmp
LDFLAGS=-lhiredis -lnetsnmp


all: myprogram

myprogram: main.o redis_task.o snmp_task.o helper.o
	$(CC) -o myprogram main.o redis_task.o snmp_task.o helper.o $(LDFLAGS)

src/main.o: src/main.c
	$(CC) -c src/main.c $(CFLAGS)

src/redis_task.o: src/redis_task.c include/redis_task.h
	$(CC) -c src/redis_task.c $(CFLAGS)

src/snmp_task.o: src/snmp_task.c include/snmp_task.h
	$(CC) -c src/snmp_task.c $(CFLAGS)

src/helper.o: src/helper.c include/helper.h
	$(CC) -c src/helper.c $(CFLAGS)

clean:
	rm -f src/*.o myprogram
