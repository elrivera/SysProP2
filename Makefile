CC = gcc
CFLAGS = -std=c99

mysh: mysh.o tok.o arraylist.o
	$(CC) $^ -o $@ 

main: tok.o arraylist.o main.o
	$(CC) -o $@ $^

par: tok.o arraylist.o par.o
	$(CC) -o $@ $^


mysh.o: tok.h arraylist.h
tok.o: tok.h
arraylist.o: arraylist.h


%.o: %.c
	$(CC) -c $< -o $@ 