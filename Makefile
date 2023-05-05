CC = gcc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -lm

implementation: implementation.c
	$(CC) $(CFLAGS) implementation.c $(LDFLAGS)

clean:
	rm -f *.o

