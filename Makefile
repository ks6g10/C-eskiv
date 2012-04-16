CC=gcc
CFLAGS=
LIBS = -lX11 -lGL -lGLU -lglut

all:eskiv

eskiv : eskiv.o
	$(CC) $(LIBS) -o eskiv eskiv.o 

eskiv.o : eskiv.c
	$(CC) $(CFLAGS) $(LIBS)  -c eskiv.c