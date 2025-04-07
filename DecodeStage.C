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
#include "DecodeStage.h"
#include "Status.h"
#include "Debug.h"


bool DecodeStage::doClockLow(PipeReg **pregs, Stage **stages) {
    
    D * dreg = (D *) pregs[DREG];
    E * ereg = (E *) pregs[EREG];

    // Grabs initialized values from previous stage's setInput method.
    // Everything here is used in Decode stage, grabbed from previous stage.

    uint64_t stat = dreg->getstat()->getOutput();
    uint64_t icode = dreg->geticode()->getOutput();
    uint64_t ifun = dreg->getifun()->getOutput();
    uint64_t rA = dreg->getrA()->getOutput();
    uint64_t rB = dreg->getrB()->getOutput();
    uint64_t valC = dreg->getvalC()->getOutput();
    uint64_t valP = dreg->getvalP()->getOutput();
    
    // values that are used in next stage that we will calculate later

    uint64_t valA = 0;
    uint64_t valB = 0;
    uint64_t dstE = RNONE;
    uint64_t dstM = RNONE;
    uint64_t srcA = SAOK;
    uint64_t srcB = SAOK;

    // initialize inputs for next stage, based on what next stage needs
    setEinput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB);
    return false;
}

void DecodeStage::doClockHigh(PipeReg **pregs) 
{
   E * ereg = (E *) pregs[EREG];

   ereg->getstat()->normal();
   ereg->geticode()->normal();
   ereg->getifun()->normal();
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

