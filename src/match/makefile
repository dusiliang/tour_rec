ifdef db
	FLAG = -Wall --static -g
else
	FLAG = -Wall --static -O2
endif
CC = g++
INCLUDE = -I../comm -I../libjson
OBJECTS = ../comm/*.o
LD_PATH = -L../libjson
LIBS = -ljson -lpthread

all: match 

match: json $(OBJECTS)
	$(CC) $(FLAG) $(OBJECTS) match.cpp -o $@ $(INCLUDE) $(LD_PATH) $(LIBS)

json:
	make -C ../libjson

../comm/*.o: ../comm/*.c* ../comm/*.h
	make -C ../comm

clean:
	make clean -C ../comm
	make clean -C ../libjson
	rm match
