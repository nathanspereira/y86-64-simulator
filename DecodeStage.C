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


bool DecodeStage::doClockLow(PipeReg **pregs, Stage **stages) {
    
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

//tell this method how to grab variabels from other classes (M_dstE, e_valE)
uint64_t getD_valA(uint64_t d_srcA, uint64_t d_rvalA, M * mreg, W * wreg, ExecuteStage * e)
{
   
   //You need to pass pointers to the M and W registers to the forwarding methods and access them out of those registers

   uint64_t M_dstE = mreg -> getdstE() -> getOutput();
   uint64_t M_valA = mreg -> getvalA() -> getOutput();
   uint64_t W_dstE = wreg -> getdstE() -> getOutput();
   uint64_t W_valE = wreg -> getvalE() -> getOutput();

   // Forwarding methods to your DecodeStage can use a pointer to the ExecuteStage object to call these methods
   uint64_t e_dstE = e -> ExecuteStage::getE_dstE();
   uint64_t e_valE = e -> ExecuteStage::getE_valE();


   RegisterFile * regInstance = RegisterFile::getInstance();
   bool error = false;
   if (d_srcA == e_dstE) return e_valE;
   else if (d_srcA == M_dstE) return M_valA;
   else if (d_srcA == W_dstE) return W_valE;
   else return (regInstance -> RegisterFile::readRegister(d_rvalA, error)); // Where does d_rvalA come from?? "value from register file"
}

uint64_t getD_valB(uint64_t d_srcB, uint64_t d_rvalB, M * mreg, W * wreg, ExecuteStage * e )
{
   //You need to pass pointers to the M and W registers to the forwarding methods and access them out of those registers
   
   uint64_t M_dstE = mreg -> getdstE() -> getOutput();
   uint64_t M_valE = mreg -> getvalE() -> getOutput();
   uint64_t W_dstE = wreg -> getdstE() -> getOutput();
   uint64_t W_valE = wreg -> getvalE() -> getOutput();

   // Forwarding methods to your DecodeStage can use a pointer to the ExecuteStage object to call these methods
   uint64_t e_dstE = e ->getE_dstE();
   uint64_t e_valE = e -> getE_valE();

   RegisterFile * regInstance = RegisterFile::getInstance();
   bool error = false;
   if (d_srcB == e_dstE) return e_valE;
   else if (d_srcB == M_dstE) return M_valE;
   else if (d_srcB == W_dstE) return W_valE;
   else return (regInstance -> RegisterFile::readRegister(d_rvalB, error)); // where does this come from ??
}
