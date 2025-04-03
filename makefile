CC = g++
CFLAGS = -g -c -Wall -std=c++11 -O0
OBJ = yess.o Loader.o Memory.o ConditionCodes.o RegisterFile.o Tools.o FetchStage.o DecodeStage.o ExecuteStage.o MemoryStage.o WritebackStage.o Simulate.o PipeReg.o PipeRegField.o F.o D.o E.o M.o W.o Stage.o


.C.o:
	$(CC) $(CFLAGS) $< -o $@

yess: $(OBJ)

yess.o: Simulate.h Stage.h PipeReg.h F.h D.h E.h M.h W.h FetchStage.h DecodeStage.h ExecuteStage.h MemoryStage.h WritebackStage.h

Loader.o: Loader.h Memory.h

ConditionCodes.o: ConditionCodes.h Tools.h

Memory.o: Memory.h Tools.h

RegisterFile.o: RegisterFile.h

Tools.o: Tools.h

FetchStage.o: FetchStage.h Stage.h PipeReg.h F.h D.h E.h M.h W.h

DecodeStage.o: DecodeStage.h Stage.h PipeReg.h

ExecuteStage.o: ExecuteStage.h Stage.h PipeReg.h

MemoryStage.o: MemoryStage.h Stage.h PipeReg.h

WritebackStage.o: WritebackStage.h Stage.h PipeReg.h

Simulate.o: Simulate.h Stage.h PipeReg.h F.h D.h E.h M.h W.h FetchStage.h DecodeStage.h ExecuteStage.h MemoryStage.h WritebackStage.h

PipeReg.o: PipeReg.h PipeRegField.h

PipeRegField.o: PipeRegField.h

F.o: F.h PipeReg.h PipeRegField.h

D.o: D.h PipeReg.h PipeRegField.h

E.o: E.h PipeReg.h PipeRegField.h

M.o: M.h PipeReg.h PipeRegField.h

W.o: W.h PipeReg.h PipeRegField.h

Stage.o: Stage.h PipeReg.h

clean:
	rm $(OBJ) -o yess

run:
	make clean
	make yess
	./run.sh

removeR:
	sed -i -e 's/\r$$//' *



