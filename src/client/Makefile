############################# Makefile ##########################
CFLAGS= -W -Wall -Wextra -O2 -D_GNU_SOURCE

UTILS=../utils
DOC = ../docs
OBJECTS:= client.o ../utils/utils.o
EXECUTAVEL=cv

$(EXECUTAVEL): $(OBJECTS)
	cc -o $(EXECUTAVEL) $(OBJECTS) $(CFLAGS)
	mv $(EXECUTAVEL) ..

run:
	./$(EXECUTAVEL)

clean:
	rm -rf *.o $(UTILS)/*.o ../$(EXECUTAVEL) $(DOC)

grind:
	valgrind --leak-check=full --show-reachable=no --show-leak-kinds=all ./$(EXECUTAVEL)
  
client.o: client.c ../utils/utils.o
