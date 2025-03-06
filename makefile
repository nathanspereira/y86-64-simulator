CC = g++
CFLAGS = -g -c -Wall -std=c++11 -O0
OBJ = lab5.h


.C.o:
	$(CC) $(CFLAGS) $< -o $@

lab5: $(OBJ) 

Loader.o: Loader.h

Tools.o : Tools.h


clean:
	rm $(OBJ) lab4

run:
	make clean
	make lab4
	./run.sh

removeR:
	sed -i -e 's/\r$$//' *