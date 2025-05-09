#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "WritebackStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"


bool WritebackStage::doClockLow(PipeReg **pregs, Stage **stages) 
{
    W * wreg = (W *) pregs[WREG];
    uint64_t icode = wreg -> geticode()-> getOutput();
    //uint64_t valE = wreg->getvalE()->getOutput();
    //uint64_t dstE = wreg->getdstE()->getOutput();
    //uint64_t valM = wreg ->getvalM()->getOutput();
    
    return icode == IHALT;
}


void WritebackStage::doClockHigh(PipeReg **pregs)
{
    W * wreg = (W *) pregs[WREG];
    uint64_t valE = wreg->getvalE()->getOutput();
    uint64_t dstE = wreg->getdstE()->getOutput();

    uint64_t dstM = wreg->getdstM()->getOutput(); 
    uint64_t valM = wreg->getvalM()->getOutput(); 

    RegisterFile * reg = RegisterFile::getInstance();
    bool alwaysFalse = false; 
    bool alwaysFalseTwo = false;

    reg->RegisterFile::writeRegister(valE, dstE, alwaysFalse);
    reg->RegisterFile::writeRegister(valM, dstM, alwaysFalseTwo);
}
