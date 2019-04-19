CFLAGS= -W -Wall -Wextra -O2
EXECUTAVEL = ma
OBJECTS:= manager.o article.o ../utils/utils.o

$(EXECUTAVEL): $(OBJECTS)
	cc $(CFLAGS) -o $(EXECUTAVEL) $(OBJECTS)

clean:
	rm -rf artigos strings $(EXECUTAVEL) *.o

article.o: article.c article.h
manager.o: manager.c article.o ../utils/utils.o
