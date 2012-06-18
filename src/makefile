ifdef db
	FLAG = -Wall --static -g
else
	FLAG = -Wall --static -O2
endif
CC = g++
INCLUDE = -I../api -Icomm -Ilibjson
OBJECTS = comm/*.o
LD_PATH = -L../api -Llibjson
LIBS = -lICTCLAS50 -lpthread -ljson

all: parse_data

parse_data: json $(OBJECTS)
	$(CC) $(FLAG) $(OBJECTS) parse_data.cpp -o $@ $(INCLUDE) $(LD_PATH) $(LIBS)

json:
	make -C libjson

comm/*.o: comm/*.c* comm/*.h
	make -C comm

clean:
	make clean -C comm
	make clean -C libjson
	rm parse_data 
