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
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "ConditionCodes.h"
#include "Tools.h"



bool ExecuteStage::doClockLow(PipeReg **pregs, Stage **stages) {
   E * ereg = (E *) pregs[EREG];
   M * mreg = (M *) pregs[MREG];
   
   // values from previous stage that are needed for current stage
   uint64_t stat = ereg->getstat()->getOutput();
   uint64_t icode = ereg->geticode()->getOutput();
   uint64_t ifun = ereg->getifun()->getOutput();
   uint64_t valC = ereg->getvalC()->getOutput();
   uint64_t valA = ereg->getvalA()->getOutput(); 
   uint64_t valB = ereg->getvalB()->getOutput(); 
   uint64_t dstE = ereg->getdstE()->getOutput();
   uint64_t dstM = ereg->getdstM()->getOutput();   
   uint64_t srcA = ereg->getsrcA()->getOutput();   
   uint64_t srcB = ereg->getsrcB()->getOutput(); 

   uint64_t Cnd = 0; // why are we setting this to 0 ??

   // alu related variables
   uint64_t aluA = getAluA(icode, valA, valC);
   uint64_t aluB = getAluB(icode, valB);
   uint64_t aluFun = getAluFun(icode, ifun); 
   e_valE = aluLogicCircuit(aluA, aluB, aluFun);

   bool ccChanged = set_cc(icode);
   e_dstE = set_dstE(icode, Cnd, dstE);

   
   bool falsy = false;

   ccLogicCircuit(ccChanged, aluA,  aluB,  aluFun, e_valE, falsy);
   setMinput(mreg, stat, icode, Cnd, e_valE, valA, e_dstE, dstM);
   return false;
}

void ExecuteStage::doClockHigh(PipeReg **pregs) 
{
   
   M * mreg = (M *) pregs[MREG];
   mreg->getstat()->normal();
   mreg->geticode()->normal();
   mreg->getCnd()->normal();
   mreg->getvalE()->normal();
   mreg->getvalA()->normal();
   mreg->getdstE()->normal();
   mreg->getdstM()->normal();
}

void ExecuteStage::setMinput(M * mreg, uint64_t stat, uint64_t icode, 
                           uint64_t Cnd, uint64_t valE, uint64_t valA, uint64_t dstE, uint64_t dstM)
{
   mreg->getstat()->setInput(stat);
   mreg->geticode()->setInput(icode);
   mreg->getCnd()->setInput(Cnd);
   mreg->getvalE()->setInput(valE);
   mreg->getvalA()->setInput(valA);
   mreg->getdstE()->setInput(dstE);
   mreg->getdstM()->setInput(dstM);
}

//From HCL
uint64_t ExecuteStage::getAluA(uint64_t E_icode, uint64_t E_valA, uint64_t E_valC)
{
   if (E_icode == IRRMOVQ || E_icode == IOPQ) return E_valA;
   else if (E_icode == IIRMOVQ || E_icode == IRMMOVQ || E_icode == IMRMOVQ) return E_valC;
   else if (E_icode == ICALL || E_icode == IPUSHQ) return  -8;
   else if (E_icode == IRET || E_icode == IPOPQ) return  8;
   else return 0;
}

//From HCL
uint64_t ExecuteStage::getAluB(uint64_t E_icode, uint64_t E_valB)
{ 
   if (E_icode == IRMMOVQ || E_icode == IMRMOVQ || E_icode == IOPQ || E_icode == ICALL 
                          || E_icode == IPUSHQ || E_icode == IRET || E_icode == IPOPQ) 
      return E_valB;

   else if (E_icode == IRRMOVQ || E_icode == IIRMOVQ) return  0;
   else return 0;
}

//from HCL
uint64_t ExecuteStage::getAluFun(uint64_t E_icode, uint64_t E_ifun)
{
   if (E_icode == IOPQ) return E_ifun;
   else return ADDQ;
}

bool ExecuteStage::set_cc(uint64_t E_icode)
{
   return E_icode == IOPQ;
}

////HCL turned into C++ for dstE component
uint64_t ExecuteStage::set_dstE(uint64_t E_icode, uint64_t e_Cnd, uint64_t dstE)
{
   if ((E_icode == IRRMOVQ) && !(e_Cnd)) 
   {
      return RNONE;
   }
   else
   {
      return dstE;
   }
 
}  

// If the set_cc component returns true then the CC component will be used to set the condition codes 
// (in the ConditionCodes class) to 0 or 1. 
// Probably NOT where the problem is 
void ExecuteStage::ccLogicCircuit(bool ccChanged, uint64_t aluA, uint64_t aluB,  uint64_t aluFun, uint64_t E_valE, bool falsy)
{
   ConditionCodes * codes = ConditionCodes::getInstance();
   
   if (ccChanged)
   {
      bool overflow = (Tools::addOverflow(aluA, aluB) && aluFun == ADDQ )|| (Tools::subOverflow(aluA, aluB) && aluFun == SUBQ);
      codes->ConditionCodes::setConditionCode(overflow, OF, falsy); 

      bool sign = Tools::sign(E_valE);
      codes->ConditionCodes::setConditionCode(sign, SF , falsy);

      bool zerFlag = (E_valE == 0);
      codes->ConditionCodes::setConditionCode(zerFlag, ZF , falsy);
      
   }

}

// The ALU will be used to perform add, sub, xor, or and depending upon the value returned from the ALU fun. control unit.
//   See the ExecuteStage diagram to see how all of these fit together.
// Multiplexer, + - ^ &
uint64_t ExecuteStage::aluLogicCircuit(uint64_t aluA, uint64_t aluB, uint64_t aluFun)
{
   if (aluFun == SUBQ)
   {
      return aluB - aluA;
   }
   else if (aluFun == XORQ)
   {
      return aluB ^ aluA;
   }
   else if (aluFun == ANDQ)
   {
      return aluB & aluA;
   }

   else return aluB + aluA;
}

uint64_t ExecuteStage::gete_dstE()
{
   return e_dstE;
}

uint64_t ExecuteStage::gete_valE()
{
   return e_valE;
}

//Lab 9
bool ExecuteStage::cond(uint64_t E_icode, uint64_t E_ifun) 
{
   bool falsy = false;
   if (E_icode != IJXX && E_icode != ICMOVXX) 
   {
      return 0; 
   }
   ConditionCodes * codes = ConditionCodes::getInstance();
   bool zf = codes ->ConditionCodes::getConditionCode(ZF, falsy);
   bool sf = codes ->ConditionCodes::getConditionCode(SF, falsy);
   bool of = codes ->ConditionCodes::getConditionCode(OF, falsy);

   switch (E_ifun)
   {
      case (UNCOND): return 1;

      // jle/cmovle	(sf ^ of) | zf	less than or equal to zero
      case (LESSEQ): if ((sf ^ of) || zf) return 1;

      // jl/cmovl	(sf ^ of)	less than zero
      case (LESS): if (sf ^ of) return 1;

      // je/cmove	zf	equal to zero
      case (EQUAL): if (zf) return 1;

      // jne/cmovne	!zf	not equal to zero
      case (NOTEQUAL): if (!zf) return 1;

      // jg/cmovg	!(sf ^ of) & !zf	greater than zero
      case (GREATEREQ): if (!(sf ^ of) && !zf) return 1;

      // jge/cmovge	!(sf ^ of) greater than or equal to zero
      case (GREATER): if (!(sf ^ of)) return 1;

      default: return 0;
   }
   

}

