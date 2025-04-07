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
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"


bool MemoryStage::doClockLow(PipeReg **pregs, Stage **stages) 
{
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];

   //grabs values from previous stage for current stage

   uint64_t stat = mreg->getstat()->getOutput();
   uint64_t icode = mreg->geticode()->getOutput();
   uint64_t Cnd = mreg -> getCnd()->getOutput();   //will calculate later, holds 0 currently
   uint64_t valE = mreg ->getvalE()->getOutput();  //will calculate later, holds 0 currently
   uint64_t valA = mreg ->getvalA()->getOutput();
   uint64_t dstE = mreg ->getdstE()->getOutput();
   uint64_t dstM = mreg->getdstM()->getOutput();   //will calculate later, holds RNONE currently

   uint64_t valM = mreg->getvalA()->getOutput() & 0; // will calculate later using valA. Currently holds 0

   //sets inputs for next stage
   setWinput(wreg, stat, icode, valE, valM, dstE, dstM);
   return false;
}

void MemoryStage::doClockHigh(PipeReg **pregs)
{
   W * wreg = (W *) pregs[WREG];
   wreg->getstat()->normal();
   wreg->geticode()->normal();
   wreg->getvalE()->normal();
   wreg->getvalM()->normal();
   wreg->getdstE()->normal();
   wreg->getdstM()->normal();
}

// initailizes values for next stage
void MemoryStage::setWinput(W * wreg, uint64_t stat, uint64_t icode,
                           uint64_t valE, uint64_t valM, uint64_t dstE, uint64_t dstM)
{
   wreg->getstat()->setInput(stat);
   wreg->geticode()->setInput(icode);
   wreg->getvalE()->setInput(valE);
   wreg->getvalM()->setInput(valM);
   wreg->getdstE()->setInput(dstE);
   wreg->getdstM()->setInput(dstM);
}

