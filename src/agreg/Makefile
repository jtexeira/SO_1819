############################# Makefile ##########################
CFLAGS= -W -Wall -Wextra -O2 -D_GNU_SOURCE
UTILS=../utils
DOC = ../docs
OBJECTS:= agreg.o $(UTILS)/utils.o
EXECUTAVEL=ag

$(EXECUTAVEL): $(OBJECTS)
	cc -o $(EXECUTAVEL) $(OBJECTS) $(CFLAGS)
	mv $(EXECUTAVEL) ..

run:
	./$(EXECUTAVEL)

clean:
	rm -rf stocks vendas ../$(EXECUTAVEL) *.o

grind:
	valgrind --leak-check=full --show-reachable=no --show-leak-kinds=all ./$(EXECUTAVEL)

agreg.o: agreg.c $(UTILS)/utils.o 
