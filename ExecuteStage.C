#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"

bool ExecuteStage::doClockLow(PipeReg **pregs, Stage **stages) {
   E * ereg = (E *) pregs[EREG];
   M * mreg = (M *) pregs[MREG];
   
   // values from previous stage that are needed for current stage
   uint64_t stat = ereg->getstat()->getOutput();
   uint64_t icode = ereg->geticode()->getOutput();
   uint64_t ifun = ereg->getifun()->getOutput();
   uint64_t valC = ereg->getvalC()->getOutput();

   uint64_t valA = ereg->getvalA()->getOutput(); // currently set to 0
   uint64_t valB = ereg->getvalB()->getOutput(); // currently set to 0)

   
   uint64_t dstE = ereg-> getdstE() -> getOutput(); // currently set to RNONE
   uint64_t dstM = ereg->getdstM()->getOutput();   // currently set to RNONE
   uint64_t srcA = ereg->getsrcA()->getOutput();   // currently set to SAOK
   uint64_t srcB = ereg->getsrcB()->getOutput();   // currently set to SAOK
   

   //needed for next register, will calculate later
   uint64_t Cnd = 0; 
   uint64_t valE = 0;


   setMinput(mreg, stat, icode, Cnd, valE, valA, dstE, dstM);
   return false;
}

void ExecuteStage::doClockHigh(PipeReg **pregs) 
{
   
   M * mreg = (M *) pregs[MREG];
   mreg->getstat()->normal();
   mreg->geticode()->normal();
   mreg->getCnd()->normal();
   mreg->getvalE()->normal();
   mreg->getvalA()->normal();
   mreg->getdstE()->normal();
   mreg->getdstM()->normal();
}

void ExecuteStage::setMinput(M * mreg, uint64_t stat, uint64_t icode, 
                           uint64_t Cnd, uint64_t valE, uint64_t valA, uint64_t dstE, uint64_t dstM)
{
   mreg->getstat()->setInput(stat);
   mreg->geticode()->setInput(icode);
   mreg->getCnd()->setInput(Cnd);
   mreg->getvalE()->setInput(valE);
   mreg->getvalA()->setInput(valA);
   mreg->getdstE()->setInput(dstE);
   mreg->getdstM()->setInput(dstM);
}

