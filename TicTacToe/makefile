CC = gcc
CFLAGS = -Wall -Wfatal-errors -Werror
EXEC = tictactoe
SRC= morpion.c int_list.c qlearning_morpion.c main.c
OBJ= $(SRC:.c=.o)


all : $(EXEC)

tictactoe : $(OBJ)
	$(CC) -o $@ $^ 

qlearning_morpion.o : morpion.h int_list.h 

main.o : qlearning_morpion.h

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o
