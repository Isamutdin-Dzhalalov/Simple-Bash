CC=gcc
CFLAGS=-c -Wall -Werror -Wextra
EXECUTABLE=s21_cat

all: $(EXECUTABLE)
	@rm *.o

$(EXECUTABLE): s21_cat.o 
	$(CC) s21_cat.o  -o $(EXECUTABLE)

s21_grep.o: s21_cat.c s21_cat.h 
	$(CC) $(CFLAGS) -c s21_cat.c
	
clean:
	rm -rf *.o 

fclean:
	@rm $(EXECUTABLE)

rebuild: fclean all

test: 
	sh test_func_cat.sh

check:
	sh test_leak_cat.sh

.PHONY:	all clean fclean rebuild
