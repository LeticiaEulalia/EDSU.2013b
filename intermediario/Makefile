CFLAGS=-Wall -g

all: intermediario

intermediario.o: comun.h
comun.o: comun.h

intermediario: intermediario.o comun.o
	$(CC) -g -o $@ intermediario.o comun.o -lpthread

clean:
	rm -f *.o intermediario
