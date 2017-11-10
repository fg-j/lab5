
CC = gcc
CFLAGS = -Og -Wall

ringbuf: ringbuf.o  
	$(CC) $(CFLAGS) -o ringbuf ringbuf.o -lpthread

ringbuf.o: ringbuf.c
	$(CC) $(CFLAGS) -c ringbuf.c 

clean : 
	rm ringbuf ringbuf.o
