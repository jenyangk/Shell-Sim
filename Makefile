CC      = gcc
CFLAGS  = -Wall -O2
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)

.PHONY: all clean

all: dragonshell

clean:
	rm *.o dragonshell

compress:
	zip A1.zip dragonshell.c forking.c functions.c Makefile README.md

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@ -g

dragonshell: $(OBJECTS)
	$(CC) -o dragonshell $(OBJECTS)
