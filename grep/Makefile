CC=gcc
CFLAGS=-c -Wall -Werror -Wextra
EXECUTABLE=s21_grep

all: $(EXECUTABLE)
	@rm *.o

$(EXECUTABLE): s21_grep.o 
	$(CC) s21_grep.o  -o $(EXECUTABLE)

s21_grep.o: s21_grep.c s21_grep.h 
	$(CC) $(CFLAGS) -c s21_grep.c
	
clean:
	rm -rf *.o 

fclean:
	@rm $(EXECUTABLE)

rebuild: fclean all

test: 
	sh test_func_grep.sh

check:
	sh test_leak_grep.sh

.PHONY:	all clean fclean rebuild
