#include <string>
#include <cstdint>
#include "Instructions.h"
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
#include "MemoryStage.h"
#include "DecodeStage.h"
#include "Debug.h"


bool DecodeStage::doClockLow(PipeReg **pregs, Stage **stages) 
{
   D * dreg = (D *) pregs[DREG];
   E * ereg = (E *) pregs[EREG];
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];
   ExecuteStage * e = (ExecuteStage *) stages[ESTAGE];

    // Grabs initialized values from previous stage's setInput method.
    // Everything here is used in Decode stage, grabbed from previous stage.

   uint64_t stat = dreg->getstat()->getOutput();
   uint64_t icode = dreg->geticode()->getOutput();
   uint64_t ifun = dreg->getifun()->getOutput();
   uint64_t rA = dreg->getrA()->getOutput();
   uint64_t rB = dreg->getrB()->getOutput();
   uint64_t valC = dreg->getvalC()->getOutput();
   uint64_t valP = dreg->getvalP()->getOutput();
   
   // Forwarding methods to your DecodeStage can use a pointer to the ExecuteStage object to call these methods
   uint64_t dstE = e -> ExecuteStage::getE_dstE(icode, 0, dstE);
   uint64_t valE = e -> ExecuteStage::getE_valE();

   uint64_t dstM = builddstM(icode, rA); //(uint64_t D_icode, uint64_t rA)
   uint64_t srcA = buildsrcA(icode, rA); //(uint64_t D_icode, uint64_t rA)
   uint64_t srcB = buildsrcB(icode, rB);
   
   RegisterFile * regInstance = RegisterFile::getInstance();
   bool error = false;
   uint64_t d_rvalA = regInstance -> RegisterFile::readRegister(srcA, error);
   uint64_t d_rvalB = regInstance -> RegisterFile::readRegister(srcB, error);

   uint64_t valA = getD_valA(srcA, d_rvalA, mreg, wreg, dstE, valE);
   uint64_t valB = getD_valB(srcB, d_rvalB, mreg, wreg, e, dstE, valE);



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

void DecodeStage::setEinput(E * ereg, uint64_t stat, uint64_t icode, uint64_t ifun, uint64_t valC, uint64_t valA, 
                                             uint64_t valB, uint64_t dstE, uint64_t dstM, uint64_t srcA, uint64_t srcB)
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


//HCL for srcA component
uint64_t DecodeStage::buildsrcA(uint64_t D_icode, uint64_t rA)
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

uint64_t DecodeStage::buildsrcB(uint64_t D_icode, uint64_t rB)
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


uint64_t DecodeStage::builddstE(uint64_t D_icode, uint64_t rB)
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

uint64_t DecodeStage::builddstM(uint64_t D_icode, uint64_t rA)
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


//tell this method how to grab variabels from other classes (M_dstE, e_valE)
uint64_t getD_valA(uint64_t d_srcA, uint64_t d_rvalA, M * mreg, W * wreg, uint64_t e_dstE, uint64_t e_valE)
{
   
   //You need to pass pointers to the M and W registers to the forwarding methods and access them out of those registers

   uint64_t M_dstE = mreg -> getdstE() -> getOutput();
   uint64_t M_valA = mreg -> getvalA() -> getOutput();
   uint64_t W_dstE = wreg -> getdstE() -> getOutput();
   uint64_t W_valE = wreg -> getvalE() -> getOutput();

   if (d_srcA == e_dstE) return e_valE;
   else if (d_srcA == M_dstE) return M_valA;
   else if (d_srcA == W_dstE) return W_valE;
   else return d_rvalA; // Where does d_rvalA come from?? "value from register file"
}

uint64_t getD_valB(uint64_t d_srcB, uint64_t d_rvalB, M * mreg, W * wreg, ExecuteStage * e, uint64_t e_dstE, uint64_t e_valE)
{
   //You need to pass pointers to the M and W registers to the forwarding methods and access them out of those registers
   
   uint64_t M_dstE = mreg -> getdstE() -> getOutput();
   uint64_t M_valE = mreg -> getvalE() -> getOutput();
   uint64_t W_dstE = wreg -> getdstE() -> getOutput();
   uint64_t W_valE = wreg -> getvalE() -> getOutput();




   if (d_srcB == e_dstE) return e_valE;
   else if (d_srcB == M_dstE) return M_valE;
   else if (d_srcB == W_dstE) return W_valE;
   else return d_rvalB; // where does this come from ??
}

