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
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"


bool MemoryStage::doClockLow(PipeReg **pregs, Stage **stages) {

    M * Mreg = (M *) pregs[MREG];
    W * Wreg = (W *) pregs[WREG];

    uint64_t valM = RNONE;
    setMinput(wreg, stat, icode, valE, valM, dstE, dstM);
    return false;
}

void MemoryStage::doClockHigh(PipeReg **pregs)
{

   M * mreg = (M *) pregs[MREG];

   mreg->getpredPC()->normal();
   wreg->getstat()->normal();
   wreg->geticode()->normal();
   wreg->getvalE()->normal();
   wreg->getvalM()->normal();
   wreg->getdstE()->normal();
   wreg->getdstM()->normal();
}

void MemoryStage::setMinput(W * wreg, uint64_t stat, uint64_t icode,
                           uint64_t valE, uint64_t valM, uint64_t dstE, uint64_t dstM)
{
   wreg->getstat()->setInput(stat);
   wreg->geticode()->setInput(icode);
   wreg->getvalE()->setInput(valE);
   wreg->getvalM()->setInput(valM);
   wreg->getdstE()->setInput(dstE);
   wreg->getdstM()->setInput(dstM);
}

