CC = gcc
CFLAGS = -W -Wall 

GTKFLAGS=`pkg-config --cflags gtk+-3.0`
GTKLIBS= `pkg-config --libs gtk+-3.0`

EXEC = projet_reseaux

all: $(EXEC)

trame.o: trame.c trame.h
	$(CC) $(CFLAGS)  -c trame.c

interface.o: interface.c interface.h 
	$(CC) $(CFLAGS) $(GTKFLAGS) -c interface.c $(GTKLIBS)

projet_reseaux.o: projet_reseaux.c trame.h interface.h 
	$(CC) $(CFLAGS) $(GTKFLAGS) -c projet_reseaux.c 
	
projet_reseaux: projet_reseaux.o trame.o interface.o 
	$(CC)  -o projet_reseaux projet_reseaux.o trame.o interface.o  $(GTKLIBS)
	
clean:
	rm -f *.o $(EXEC)