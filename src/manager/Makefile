############################# Makefile ##########################
CFLAGS= -W -Wall -Wextra -O2
UTILS=../utils
DOC = ../docs
OBJECTS:= manager.o article.o $(UTILS)/utils.o
EXECUTAVEL=ma

$(EXECUTAVEL): $(OBJECTS)
	cc -o $(EXECUTAVEL) $(OBJECTS) $(CFLAGS)

run:
	./$(EXECUTAVEL)

clean:
	rm -rf ARTIGOS STRINGS $(EXECUTAVEL) *.o

grind:
	valgrind --leak-check=full --show-reachable=no --show-leak-kinds=all ./$(EXECUTAVEL)
  
article.o: article.c article.h
manager.o: manager.c article.o $(UTILS)/utils.o

