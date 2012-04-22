CC=gcc
CFLAGS= -g
LIBS = -lX11 -lGL -lGLU -lglut

all:eskiv

eskiv : eskiv.o
	$(CC) $(LIBS) -o eskiv eskiv.o 

eskiv.o : eskiv.c light.h
	$(CC) $(CFLAGS) $(LIBS)  -c eskiv.c light.h