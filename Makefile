CC = gcc
FLAGS = -Wall -Werror -Wextra

all:
	clear
	$(CC) $(FLAGS) -o test main.c libtesting/*.c -lm

memtest:
	clear
	$(CC) $(FLAGS) -g -o test main.c libtesting/*.c -lm
	valgrind -s --tool=memcheck --leak-check=yes ./test
