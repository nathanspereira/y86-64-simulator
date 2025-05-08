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
#include "Instructions.h"
#include "Memory.h"


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

   uint64_t address = Addr(icode, valE, valA);
   Memory *mem = Memory::getInstance();
   
   bool error = false;

   if (mem_read(icode))
   {
      valM = mem -> Memory::getLong(address, error);
   }

   if (mem_write(icode))
   {
      mem -> Memory::putLong(valA, address, error);
   }
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

// HCL for Addr component
// word mem_addr = [
// M_icode in { IRMMOVQ, IPUSHQ, ICALL, IMRMOVQ } : M_valE;
// M_icode in { IPOPQ, IRET } : M_valA;
// 1: 0;
// ];
uint64_t MemoryStage::Addr(uint64_t M_icode, uint64_t M_valE, uint64_t M_valA)
{
   if (M_icode == IRMMOVQ || M_icode == IPUSHQ || M_icode == ICALL || M_icode == IMRMOVQ)
   {
      return M_valE;
   }
   if (M_icode == IPOPQ || M_icode == IRET)
   {
      return M_valA;
   }
   else return 0;
}

//bool mem_read = M_icode in { IMRMOVQ, IPOPQ, IRET };
bool MemoryStage::mem_read(uint64_t M_icode)
{
   return (M_icode == IMRMOVQ || M_icode == IPOPQ || M_icode == IRET);
   return 0;
}

//bool mem_write = M_icode in { IRMMOVQ, IPUSHQ, ICALL };
bool MemoryStage::mem_write(uint64_t M_icode)
{
   return (M_icode == IRMMOVQ || M_icode == IPUSHQ || M_icode == ICALL);
   return 0;
}



