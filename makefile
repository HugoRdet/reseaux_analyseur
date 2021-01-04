CC = gcc
CFLAGS = -W -Wall #-Qunused-arguments

GTKFLAGS=`pkg-config --cflags gtk+-3.0`
GTKLIBS= `pkg-config --libs gtk+-3.0`

EXEC = BabyShark

all: $(EXEC)

trame.o: trame.c trame.h interface.h types.h
	$(CC) $(CFLAGS) $(GTKFLAGS) -c trame.c $(GTKLIBS)


interface.o: interface.c trame.h types.h  interface.h 
	$(CC) $(CFLAGS) $(GTKFLAGS) -c interface.c $(GTKLIBS)

BabyShark.o: BabyShark.c trame.h interface.h types.h
	$(CC) $(CFLAGS) $(GTKFLAGS) -c BabyShark.c 
	
BabyShark: BabyShark.o trame.o interface.o 
	$(CC)  -o BabyShark BabyShark.o trame.o interface.o   $(GTKLIBS)
	
	
clean:
	rm -f *.o $(EXEC)
