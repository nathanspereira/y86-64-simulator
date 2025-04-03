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
    
    
    uint64_t rA = RNONE, rB = RNONE;
    setEinput(dreg, stat, icode, ifun, rA, rB, valC, valP);
    return false;
}

void DecodeStage::doClockHigh(PipeReg **pregs) 
{
   
   E * ereg = (E *) pregs[REG];

   dreg->getpredPC()->normal();
   ereg->getstat()->normal();
   ereg->geticode()->normal();
   ereg->getifun()->normal();
   ereg->getrA()->normal();
   ereg->getrB()->normal();
   ereg->getvalC()->normal();
   ereg->getvalP()->normal();
}

void DecodeStage::setEinput(D * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP)
{
   dreg->getstat()->setInput(stat);
   dreg->geticode()->setInput(icode);
   dreg->getifun()->setInput(ifun);
   dreg->getrA()->setInput(rA);
   dreg->getrB()->setInput(rB);
   dreg->getvalC()->setInput(valC);
   dreg->getvalP()->setInput(valP);
}

