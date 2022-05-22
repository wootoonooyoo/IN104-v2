CC = gcc
CFLAGS = -Wall -Wfatal-errors -Werror
EXEC = maze
SRC= functions.c mazeEnv.c main.c
OBJ= $(SRC:.c=.o)


all : $(EXEC)

maze : $(OBJ)
	$(CC) -o $@ $^ -lm

mazeEnv.o : functions.h

main.o : functions.h mazeEnv.h

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o
