############################# Makefile ##########################
CFLAGS= -W -Wall -Wextra -O2
UTILS=../utils
DOC = ../docs
OBJECTS:= server.o ../utils/utils.o ../manager/article.o
EXECUTAVEL=sv

$(EXECUTAVEL): $(OBJECTS)
	cc -o $(EXECUTAVEL) $(OBJECTS) $(CFLAGS)

run:
	./$(EXECUTAVEL)

clean:
	rm -rf STOCKS VENDAS $(EXECUTAVEL) *.o

grind:
	valgrind --leak-check=full --show-reachable=no --show-leak-kinds=all ./$(EXECUTAVEL)

server.o: server.c ../utils/utils.o ../manager/article.o
