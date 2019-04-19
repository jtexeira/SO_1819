CFLAGS= -W -Wall -Wextra -O2
EXECUTAVEL = ma
OBJECTS:= client.o ../utils/utils.o

$(EXECUTAVEL): $(OBJECTS)
	cc $(CFLAGS) -o $(EXECUTAVEL) $(OBJECTS)

clean:
	rm -rf artigos strings $(EXECUTAVEL) *.o

client.o: client.c ../utils/utils.o
