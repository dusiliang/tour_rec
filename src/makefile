ifdef db
	FLAG = -Wall --static -g
else
	FLAG = -Wall --static -O2
endif
CC = g++
INCLUDE = -I../api -Icomm
OBJECTS = comm/*.o

all: parse_data
comm/*.o: comm/*.c* comm/*.h
	make -C comm

parse_data: $(OBJECTS) parse_data.cpp process_data.cpp
	$(CC) $(FLAG) $^ -o $@ $(INCLUDE) -L../api -lICTCLAS50 -lpthread

clean:
	make clean -C comm
	rm parse_data 
