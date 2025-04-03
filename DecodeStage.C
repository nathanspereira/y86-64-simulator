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
#include "DecodeStage.h"
#include "Status.h"
#include "Debug.h"


bool DecodeStage::doClockLow(PipeReg **pregs, Stage **stages) {
    
    D * dreg = (D *) pregs[DREG];
    E * ereg = (E *) pregs[EREG];
    
    uint64_t valA = 0, valB = 0;
    uint64_t dstE = RNONE, dstM = RNONE, srcA = SAOK, srcB = SAOK;
    setEinput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB);
    return false;
}

void DecodeStage::doClockHigh(PipeReg **pregs) 
{
   
   E * ereg = (E *) pregs[REG];

   dreg->getpredPC()->normal();
   ereg->getstat()->normal();
   ereg->geticode()->normal();
   ereg->getifun()->normal();
   //ereg->getrA()->normal();
   //ereg->getrB()->normal();
   ereg->getvalC()->normal();
}

void DecodeStage::setEinput(E * ereg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun,
                           uint64_t valC, uint64_t valA, uint64_t valB, uint64_t dstE, uint64_t dstM, uint64_t srcA, uint64_t srcB)
{
   ereg->getstat()->setInput(stat);
   ereg->geticode()->setInput(icode);
   ereg->getifun()->setInput(ifun);
   ereg->getvalC()->setInput(valC);
   ereg->getvalA()->setInput(valA);
   ereg->getvalB()->setInput(valB);
   ereg->getdstE()->setInput(dstE);
   ereg->getdstM()->setInput(dstM);
   ereg->getsrcA()->setInput(srcA);
   ereg->getsrcB()->setInput(srcB);
}

