CFLAGS= -W -Wall -Wextra -O2
EXECUTAVEL = sv
OBJECTS:= server.o ../utils/utils.o ../manager/article.o

$(EXECUTAVEL): $(OBJECTS)
	cc $(CFLAGS) -o $(EXECUTAVEL) $(OBJECTS)

clean:
	rm -rf STOCKS VENDAS $(EXECUTAVEL) *.o

server.o: server.c ../utils/utils.o ../manager/article.o
