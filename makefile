CC = gcc
CFLAGS = -W -Wall  
EXEC = projet_reseaux

all: $(EXEC)

projet_reseaux.o: projet_reseaux.c
	$(CC) $(CFLAGS) -c projet_reseaux.c

projet_reseaux: projet_reseaux.o
	$(CC) -o projet_reseaux projet_reseaux.o
	
clean:
	rm -f *.o $(EXEC)