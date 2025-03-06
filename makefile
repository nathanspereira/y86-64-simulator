CC = g++
CFLAGS = -g -c -Wall -std=c++11 -O0
OBJ = lab5.o Loader.o Memory.o ConditionCodes.o RegisterFile.o


.C.o:
	$(CC) $(CFLAGS) $< -o $@

lab5: $(OBJ) 

lab5.o : Loader.h ConditionCodes.h Memory.h RegisterFile.h

Loader.o: Loader.h

ConditionCodes.o : ConditionCodes.h

Memory.o : Memory.h

RegisterFile.o: RegisterFile.h

Tools.o : Tools.h


clean:
	rm $(OBJ) lab5

run:
	make clean
	make lab5
	./run.sh

removeR:
	sed -i -e 's/\r$$//' *