CC=gcc
CFLAGS=-Iinclude -I/usr/include/net-snmp
LDFLAGS=-lhiredis -lnetsnmp -lcjson


all: myprogram

myprogram: src/main.o src/redis_task.o src/snmp_task.o src/json_helper.o src/helper.o src/global.o
	$(CC) -o myprogram main.o redis_task.o snmp_task.o  json_helper.o helper.o global.o $(LDFLAGS)

src/main.o: src/main.c
	$(CC) -c src/main.c $(CFLAGS)

src/redis_task.o: src/redis_task.c include/redis_task.h
	$(CC) -c src/redis_task.c $(CFLAGS)

src/snmp_task.o: src/snmp_task.c include/snmp_task.h
	$(CC) -c src/snmp_task.c $(CFLAGS)

src/json_helper.o: src/json_helper.c include/json_helper.h
	$(CC) -c src/json_helper.c $(CFLAGS)

src/helper.o: src/helper.c include/helper.h
	$(CC) -c src/helper.c $(CFLAGS)

src/global.o: src/global.c include/global.h
	$(CC) -c src/global.c $(CFLAGS)

clean:
	rm -f src/*.o myprogram
