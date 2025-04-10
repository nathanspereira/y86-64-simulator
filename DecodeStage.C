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
#include "Instructions.h"


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
    uint64_t srcA = RNONE;
    uint64_t srcB = RNONE;

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
   ereg->getvalA()->normal();
   ereg->getvalB()->normal();
   ereg->getdstE()->normal();
   ereg->getdstM()->normal();
   ereg->getsrcA()->normal();
   ereg->getsrcB()->normal();

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

//D_icode in { IRRMOVQ, IRMMOVQ, IOPQ, IPUSHQ } : D_rA;
// D_icode in { IPOPQ, IRET } : RSP;
void buildSrcA()
{
   if (D_icode == IRRMOVQ || D_icode == IRMMOVQ || D_icode ==  IOPQ || D_icode == IPUSHQ)
   {
       srcA = D_rA;
   } 
   else if (D_icode == IPOPQ || D_icode == IRET)
   {
       srcA = RSP;
   }
   else
   {
       srcA = RNONE;
   }
}
// D_icode in { IOPQ, IRMMOVQ, IMRMOVQ } : D_rB;

//D_icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RSP;

//1: RNONE;   //no register needed
void buildSrcB()
{
   if (D_icode == IOPQ || D_icode == IRRMOVQ || D_icode == IMRMOVQ)
   {
      srcB = D_rB;
   }
   else if (D_icode == IPUSHQ || D_icode == IPOPQ || D_icode == ICALL || D_icode == IRET)
   {
      srcB = RSP;
   }
   else 
   {
      srcB = RNONE;
   }
   return srcB;
}

// D_icode in { IRRMOVQ, IIRMOVQ, IOPQ} : D_rB;

//D_icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RSP;

//1: RNONE;
void buildDstE()
{
   if (D_icode == IRRMOVQ || D_icode == IIRMOVQ || D_icode == IOPQ)
   {
      dstE = D_rB;
   }
   else if (D_icode == IPUSHQ || D_icode == IPOPQ || D_icode == ICALL || D_icode == IRET)
   {
      dstE = RSP;
   }
   else 
   {
      dstE = RNONE;
   }
}

//D_icode in { IMRMOVQ, IPOPQ } : D_rA;

// 1: RNONE;
void buildDstM()
{
   if (D_icode == IRRMOVQ || D_icode == IPOPQ)
   {
      dstM = D_rA;
   }
   else 
   {
      dstM = RNONE;
   }
}

void buildValA()
{
   valA = d_rvalA;
}

void buildValB()
{
   valB = d_rvalB;
}