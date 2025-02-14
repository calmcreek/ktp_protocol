# General Makefile for KTP Implementation
CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -L. -lksocket

# Targets
all: libksocket.a initksocket user1 user2

# Compile ksocket.c and create static library libksocket.a
ksocket.o: ksocket.c ksocket.h
	$(CC) $(CFLAGS) -c ksocket.c -o ksocket.o

libksocket.a: ksocket.o
	$(AR) rcs libksocket.a ksocket.o

# Compile initksocket.c into an executable
initksocket: initksocket.c libksocket.a
	$(CC) $(CFLAGS) initksocket.c -o initksocket $(LDFLAGS)

# Compile user1.c into an executable
user1: user1.c libksocket.a
	$(CC) $(CFLAGS) user1.c -o user1 $(LDFLAGS)

# Compile user2.c into an executable
user2: user2.c libksocket.a
	$(CC) $(CFLAGS) user2.c -o user2 $(LDFLAGS)

# Clean up all generated files
clean:
	rm -f *.o libksocket.a initksocket user1 user2

