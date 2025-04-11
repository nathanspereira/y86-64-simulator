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


    srcA = buildSrcA(icode, rA);
    srcB = buildSrcB(icode, rB);
    dstE = buildDstE(icode, rB);
    dstM = buildDstM(icode, rA);
    valA = buildValA(valA, srcA);
    valB = buildValB(valB, srcB);
    bool alwaysFalse = false;
    
    RegisterFile * reg = RegisterFile::getInstance();
    
    valA = reg-> RegisterFile::readRegister(srcA,  alwaysFalse);
    valB = reg->RegisterFile::readRegister(srcB,  alwaysFalse);

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
uint64_t DecodeStage::buildSrcA(uint64_t D_icode, uint64_t rA)
{
   uint64_t srcA;
   if (D_icode == IRRMOVQ || D_icode == IRMMOVQ || D_icode ==  IOPQ || D_icode == IPUSHQ)
   {
      srcA = rA;
   } 
   else if (D_icode == IPOPQ || D_icode == IRET)
   {
       srcA = RSP;
   }
   else
   {
       srcA = RNONE;
   }
   return srcA;
}
// D_icode in { IOPQ, IRMMOVQ, IMRMOVQ } : D_rB;

//D_icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RSP;

//1: RNONE;   //no register needed
uint64_t DecodeStage::buildSrcB(uint64_t D_icode, uint64_t rB)
{
   uint64_t srcB;
   if (D_icode == IOPQ || D_icode == IRRMOVQ || D_icode == IMRMOVQ)
   {
      srcB = rB;
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
uint64_t DecodeStage::buildDstE(uint64_t D_icode, uint64_t rB)
{
   uint64_t dstE;
   if (D_icode == IRRMOVQ || D_icode == IIRMOVQ || D_icode == IOPQ)
   {
      dstE = rB;
   }
   else if (D_icode == IPUSHQ || D_icode == IPOPQ || D_icode == ICALL || D_icode == IRET)
   {
      dstE = RSP;
   }
   else 
   {
      dstE = RNONE;
   }
   return dstE;
}

//D_icode in { IMRMOVQ, IPOPQ } : D_rA;

// 1: RNONE;
uint64_t DecodeStage::buildDstM(uint64_t D_icode, uint64_t rA)
{
   uint64_t dstM;
   if (D_icode == IRRMOVQ || D_icode == IPOPQ)
   {
      dstM = rA;
   }
   else 
   {
      dstM = RNONE;
   }
   return dstM;
}

uint64_t DecodeStage::buildValA(uint64_t valA, uint64_t srcA)
{
   RegisterFile * reg = RegisterFile::getInstance();
   bool alwaysFalse = false;
   return reg -> RegisterFile::readRegister(srcA,  alwaysFalse);
}

uint64_t DecodeStage::buildValB(uint64_t valB, uint64_t srcB)
{
   RegisterFile * reg = RegisterFile::getInstance();
   bool alwaysFalse = false;
   return reg->RegisterFile::readRegister(srcB,  alwaysFalse);
}
