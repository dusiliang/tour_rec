ifdef db
	FLAG = -Wall --static -g
else
	FLAG = -Wall --static -O2
endif
CC = g++
INCLUDE = -I../../api -I../comm -I../libjson
OBJECTS = ../comm/*.o
LD_PATH = -L../../api -L../libjson
LIBS = -lICTCLAS50 -lpthread -ljson

all: parse_data

parse_data: json $(OBJECTS)
	$(CC) $(FLAG) $(OBJECTS) parse_data.cpp -o $@ $(INCLUDE) $(LD_PATH) $(LIBS)

json:
	make -C ../libjson

../comm/*.o: ../comm/*.c* ../comm/*.h
	make -C ../comm

one: parse_one.cpp
	$(CC) $(FLAG) parse_one.cpp -o parse_one $(INCLUDE) $(LD_PATH) $(LIBS)

clean:
	rm parse_data
	make clean -C ../comm
	make clean -C ../libjson
