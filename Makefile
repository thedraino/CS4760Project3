LIBS =	-pthread

all: oss user

oss: oss.o
	gcc oss.o -o oss $(LIBS)

user: user.o
	gcc user.o -o user $(LIBS)

oss.o: oss.c
	gcc -c oss.c

user.o: user.c
	gcc -c user.c

clean:
	/bin/rm -f *.o oss user
