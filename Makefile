CC = gcc
CFLAGS = -g -std=c99 -Wall



%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<