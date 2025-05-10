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
#include "FetchStage.h"
#include "DecodeStage.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "Tools.h"
#include "Memory.h"

/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool FetchStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   // initializes pointers to each stages registers. 

   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];
   E * ereg = (E *) pregs[EREG];
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];

   // initializes reg values to 0 or none
   uint64_t f_pc = 0, icode = INOP, ifun = FNONE, valC = 0 , valP = 0;
   uint64_t rA = RNONE, rB = RNONE, stat = SAOK;

   //code missing here to select the value of the PC
   //and fetch the instruction from memory

   //DecodeStage * d = (DecodeStage *) stages[DSTAGE];
   //ExecuteStage * e = (ExecuteStage *) stages[ESTAGE];
   calculateControlSignals(ereg, stages, dreg, mreg);


   f_pc = selectPC(freg, mreg, wreg); // doClockLow method needs to call selectPC to obtain the value of f_pc
   
   // The needRegIds and needValC methods need to be called because 
   // the results of these are input to a method that you'll need to write called PCincrement
   bool mem_error = false;
   uint64_t mem = Memory::getInstance() -> getByte(f_pc, mem_error);
   
   if(mem_error == false ){
   	icode = Tools::getBits(mem, 4, 7);
   	ifun = Tools::getBits(mem, 0, 3);
   }

   bool need_valC = needValC(icode);
   bool needRegId = needRegIds(icode);



   //Fetching the instruction will allow the icode, ifun,
   //rA, rB, and valC to be set.
   //The lab assignment describes what methods need to be
   //written.
   //The value passed to setInput below will need to be changed

   //freg->getpredPC()->setInput(f_pc + 1);

   // The value returned by PCincrement is stored in valP
   valP = PCincrement(f_pc, needRegId, need_valC);

   valC = buildValC(f_pc, needRegId, need_valC);
   getRegIds(icode, f_pc, rA, rB);

   uint64_t F_predPC = predictPC(icode, valC, valP);
   freg -> getpredPC() -> setInput(F_predPC);

   //provide the input values for the D register
   setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);
   return false;
}

 
/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void FetchStage::doClockHigh(PipeReg ** pregs)
{
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];

   if(!F_stall){
      freg->getpredPC()-> normal();
   }
   if(!D_bubble){
      bubbleD(dreg);
   }
   else if (!D_stall){
      normalD(dreg);
   }
}

void FetchStage::normalD(D *dreg)
{
    dreg->getstat()->normal();
    dreg->geticode()->normal();
    dreg->getifun()->normal();
    dreg->getrA()->normal();
    dreg->getrB()->normal();
    dreg->getvalC()->normal();
    dreg->getvalP()->normal();
}

void FetchStage::bubbleD(D *dreg)
{
    dreg->getstat()->bubble(SAOK);
    dreg->geticode()->bubble(INOP);
    dreg->getifun()->bubble();
    dreg->getrA()->bubble(RNONE);
    dreg->getrB()->bubble(RNONE);
    dreg->getvalC()->bubble();
    dreg->getvalP()->bubble();
}

/* setDInput
 * provides the input to potentially be stored in the D register
 * during doClockHigh
 *
 * @param: dreg - pointer to the D register instance
 * @param: stat - value to be stored in the stat pipeline register within D
 * @param: icode - value to be stored in the icode pipeline register within D
 * @param: ifun - value to be stored in the ifun pipeline register within D
 * @param: rA - value to be stored in the rA pipeline register within D
 * @param: rB - value to be stored in the rB pipeline register within D
 * @param: valC - value to be stored in the valC pipeline register within D
 * @param: valP - value to be stored in the valP pipeline register within D
*/
void FetchStage::setDInput(D * dreg, uint64_t stat, uint64_t icode, 
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

uint64_t FetchStage::selectPC(F * freg, M * mreg, W * wreg)
{
   //uint64_t f_pc = 0;
   uint64_t m_icode = mreg->geticode()->getOutput();

   uint64_t m_cnd = mreg->getCnd()->getOutput();

   if (m_icode == IJXX && !m_cnd) // (M_icode == IJXX) && !(M_Cnd) : M_valA;
   {
      return mreg->getvalA()->getOutput();
   }
   else if (m_icode == IRET) // W_icode == IRET : W_valM;
   {
      return wreg->getvalM()->getOutput();
   }
   else
   {
      return freg->getpredPC()->getOutput(); // 1: F_predPC;
   }
}


bool FetchStage::needRegIds(uint64_t f_icode)
{
   return (f_icode == IRRMOVQ || f_icode ==  IOPQ || f_icode ==  IPUSHQ || f_icode ==  IPOPQ || f_icode == IIRMOVQ
   || f_icode == IRMMOVQ || f_icode == IMRMOVQ);
}

void FetchStage::getRegIds(uint64_t icode, uint64_t pc, uint64_t & rA, uint64_t & rB)
{
   if (needRegIds(icode))
   {
      bool error = false;
      uint64_t byteTwoMem = Memory::getInstance() -> getByte(pc + 1, error);
      rA = Tools::getBits(byteTwoMem, 4, 7);
      rB = Tools::getBits(byteTwoMem, 0, 3);
   }
}

bool FetchStage::needValC(uint64_t f_icode)
{
   return (f_icode == IIRMOVQ || f_icode ==  IRMMOVQ || f_icode ==  IMRMOVQ || f_icode ==  IJXX || f_icode == ICALL);
}

//  if need_valC is true, this method reads 8 bytes from memory 
// and builds and returns the valC that is then used as input to the D registe
// FIX THIS LATER W A LOOOP

uint64_t FetchStage::buildValC(uint64_t f_pc, bool needRegBool, bool need_valC)
{
   uint64_t valC = 0;
   if (need_valC)
   {
      uint8_t temp[8];
      bool error = false;
      f_pc++;
      //uint64_t valC = 0;

      if (needRegBool)
      {
         f_pc = f_pc + 1;
      }
      
      if (need_valC)
      {
         for (int32_t i = 0; i < 8; i ++)
         {
            uint64_t mem = Memory::getInstance() -> getByte(f_pc + i, error);
            temp[i] = mem;
         }
         valC = Tools::buildLong(temp) ;
         
      }
   }
   return valC;
}

//  inputs are f_icode, f_valC, f_valP
uint64_t FetchStage::predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP)
{
   uint64_t f_predPC = 0;
   if (f_icode == IJXX || f_icode == ICALL) // f_icode in { IJXX, ICALL } : f_valC;
   {
      f_predPC = f_valC;
   }
   else
   {
      f_predPC = f_valP;  //  1: f_valP;
   }
   return f_predPC;
}

// takes as input the address of the current instruction (f_pc), the result of needRegIds, 
// and the result of needValC and calculates the address of the next sequential instruction
uint64_t FetchStage::PCincrement(uint64_t f_pc, bool needRegBool, bool need_valC_bool)
{
   //calculates the next address and stores in valP
   // The value of valP is then used as input to predictPC along with the icode value and the value of valC (0 for now). 
   // The output of predictPC is the input to the F_predPC register.
   if (needRegBool)
   {
      f_pc = f_pc + 1;
   }
   if (need_valC_bool)
   {
      f_pc += 8;
   }
   
   return f_pc + 1;

}

//instr_valid method returns true if the icode is valid
bool FetchStage::instr_valid(uint64_t f_icode)
{
	return (f_icode == INOP || f_icode == IHALT || f_icode == IRRMOVQ || f_icode == IIRMOVQ || f_icode == IRMMOVQ || f_icode == IMRMOVQ || f_icode == IOPQ || f_icode == IJXX || f_icode == ICALL || f_icode == IRET || f_icode == IPUSHQ || f_icode == IPOPQ);
}

//returns the value to be stored in the stat field. It will be passed to setDInput.
//The values SADR, SINS, SHLT and SAOK are defined in Status.h.
uint64_t FetchStage::stat(bool mem_error, bool instr_valid, uint64_t f_icode)
{
	if (mem_error){
	       	return SADR;
	}
	if (!instr_valid){ 
		return SINS;
	}
	if (f_icode == IHALT){
	       	return SHLT;
	}
	else {
		return SAOK;
	}
}

void FetchStage::calculateControlSignals(E *ereg, Stage **stages, D *dreg, M *mreg)
{
    ExecuteStage * e = (ExecuteStage *) stages[ESTAGE];
    DecodeStage * d = (DecodeStage *) stages[DSTAGE];
    uint64_t E_icode = ereg->geticode()->getOutput();
    uint64_t E_dstM = ereg->getdstM()->getOutput();
    uint64_t e_Cnd = e->gete_Cnd();
    uint64_t d_srcA = d->getsrcA();
    uint64_t d_srcB = d->getsrcB(); 
    uint64_t D_icode = dreg->geticode()->getOutput();
    uint64_t M_icode = mreg->geticode()->getOutput();

    F_stall = ((E_icode == IMRMOVQ || E_icode == IPOPQ) && (E_dstM == d_srcA || E_dstM == d_srcB)) || (D_icode == IRET || E_icode == IRET || M_icode == IRET);
    D_stall = ((E_icode == IMRMOVQ || E_icode == IPOPQ) && (E_dstM == d_srcA || E_dstM == d_srcB));
    D_bubble = (E_icode == IJXX && !e_Cnd) || (!((E_icode == IMRMOVQ || E_icode == IPOPQ) && (E_dstM == d_srcA || E_dstM == d_srcB)) && ((D_icode == IRET || E_icode == IRET || M_icode == IRET)));
}
