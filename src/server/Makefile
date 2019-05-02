############################# Makefile ##########################
CFLAGS= -W -g -Wall -Wextra -O2 -D_GNU_SOURCE
UTILS=../utils
DOC = ../docs
OBJECTS:= server.o $(UTILS)/utils.o ../manager/article.o
EXECUTAVEL=sv

$(EXECUTAVEL): $(OBJECTS)
	cc -o $(EXECUTAVEL) $(OBJECTS) $(CFLAGS)
	mv $(EXECUTAVEL) ..

run:
	./$(EXECUTAVEL)

clean:
	rm -rf stocks vendas ../$(EXECUTAVEL) *.o

grind:
	valgrind --leak-check=full --show-reachable=no --show-leak-kinds=all ./$(EXECUTAVEL)

server.o: server.c $(UTILS)/utils.o ../manager/article.o
