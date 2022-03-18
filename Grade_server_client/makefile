CC=gcc
CFLAGS=-g
PFLAGS=-pthread
BINS=Gradeserver Gradeclient
OBJS=grade_server.o myqueue.o
OBJC=grade_client.o

all: $(BINS)

Gradeserver: $(OBJS)
	$(CC) $(CFLAGS) $(PFLAGS) -o $@ $^

Gradeclient: $(OBJC)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(PFLAGS) -c -o $@ $^

clean:
	rm -rf *.dSYM $(BINS)
