CC = gcc
CFLAGS = -ansi -Wall -pedantic
GTKFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTKLIBS = $(shell pkg-config --libs gtk+-3.0)
TARGET = mazerouterGUI

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

$(TARGET).exe: $(OBJ) declarations.h
	$(CC) $(CFLAGS) $(GTKFLAGS) -o $(TARGET).exe $(OBJ) $(GTKLIBS)

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) $(GTKFLAGS) -c $< $(GTKLIBS)

#settings.o: settings.c
#	$(CC) $(CFLAGS) $(GTKFLAGS) -c $< $(GTKLIBS)

%.o: %.c %.h
	$(CC) $(CFLAGS) $(GTKFLAGS) -c $< $(GTKLIBS)

clean: 
	$(RM) *.o *~

delete:
	$(RM) $(TARGET).exe *.o *~