############################# Makefile ##########################
CFLAGS= -W -Wall -Wextra -O2

UTILS=../utils
DOC = ../docs
OBJECTS:=$(patsubst %.c,%.o,$(wildcard *.c $(UTILS)/*.c))
EXECUTAVEL=cv

$(EXECUTAVEL): $(OBJECTS)
	cc -o $(EXECUTAVEL) $(OBJECTS) $(CFLAGS)

run:
	./$(EXECUTAVEL)

clean:
	rm -rf *.o $(UTILS)/*.o $(EXECUTAVEL) $(DOC)

grind:
	valgrind --leak-check=full --show-reachable=no --show-leak-kinds=all ./$(EXECUTAVEL)
