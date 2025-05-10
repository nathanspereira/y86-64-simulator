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
   MemoryStage * m = (MemoryStage *) stages[MSTAGE];

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
   uint64_t dstE = builddstE(icode, rB); //e -> ExecuteStage::gete_dstE();
   uint64_t prev_valE = e -> ExecuteStage::gete_valE();
   uint64_t prev_dstE = e -> ExecuteStage::gete_dstE();
   uint64_t m_valM = m -> MemoryStage::getValM();

   uint64_t dstM = builddstM(icode, rA); //(uint64_t D_icode, uint64_t rA)
   d_srcA = buildsrcA(icode, rA); //(uint64_t D_icode, uint64_t rA)
   d_srcB = buildsrcB(icode, rB);
   
   RegisterFile * regInstance = RegisterFile::getInstance();
   bool error = false;
   uint64_t d_rvalA = regInstance -> RegisterFile::readRegister(d_srcA, error);
   uint64_t d_rvalB = regInstance -> RegisterFile::readRegister(d_srcB, error);

   uint64_t valA = fwdA(d_srcA, d_rvalA, mreg, wreg, prev_dstE, prev_valE, valP, icode, m_valM);
   uint64_t valB = fwdB(d_srcB, d_rvalB, mreg, wreg, prev_dstE, prev_valE, m_valM);

   uint64_t E_icode = ereg->geticode()->getOutput();
   uint64_t E_dstM = ereg->getdstM()->getOutput();
   uint64_t E_cnd = e->ExecuteStage::gete_Cnd();
   calculateControlSignals(E_icode, E_dstM, E_cnd);

    // initialize inputs for next stage, based on what next stage needs
    setEinput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, d_srcA, d_srcB);
    return false;
}

void DecodeStage::doClockHigh(PipeReg **pregs) 
{
   E * ereg = (E *) pregs[EREG];

   if (E_bubble) {
      bubbleE(ereg);
   } 
   else {
      normalE(ereg);
   }   

}

void DecodeStage::normalE(E *ereg)
{
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

void DecodeStage::bubbleE(E *ereg)
{
    ereg->getstat()->bubble();
    ereg->geticode()->bubble(INOP);
    ereg->getifun()->bubble();
    ereg->getvalC()->bubble();
    ereg->getvalA()->bubble();
    ereg->getvalB()->bubble();
    ereg->getdstE()->bubble(RNONE);
    ereg->getdstM()->bubble(RNONE);
    ereg->getsrcA()->bubble(RNONE);
    ereg->getsrcB()->bubble(RNONE); 
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
   if (D_icode == IRRMOVQ || D_icode == IRMMOVQ || D_icode ==  IOPQ || D_icode == IPUSHQ)
   {
      return rA;
   } 
   else if (D_icode == IPOPQ || D_icode == IRET)
   {
      return RSP;
   }
   else
   {
      return RNONE;
   }
}

uint64_t DecodeStage::buildsrcB(uint64_t D_icode, uint64_t d_rB)
{

   if (D_icode == IOPQ || D_icode == IRMMOVQ || D_icode == IMRMOVQ)
   {
      return d_rB;
   }
   else if (D_icode == IPUSHQ || D_icode == IPOPQ || D_icode == ICALL || D_icode == IRET)
   {
      return RSP;
   }
   else 
   {
      return RNONE;
   }
}


uint64_t DecodeStage::builddstE(uint64_t D_icode, uint64_t rB)
{
   if (D_icode == IRRMOVQ || D_icode == IIRMOVQ || D_icode == IOPQ)
   {
      return rB;
   }
   else if (D_icode == IPUSHQ || D_icode == IPOPQ || D_icode == ICALL || D_icode == IRET)
   {
      return RSP;
   }
   else 
   {
      return RNONE;
   }
}

uint64_t DecodeStage::builddstM(uint64_t D_icode, uint64_t rA)
{
   if (D_icode == IMRMOVQ || D_icode == IPOPQ)
   {
      return rA;
   }
   else 
   {
      return RNONE;
   }
}

// //HCL for Sel+FwdA

// word d_valA = [
// D_icode in { ICALL, IJXX } : D_valP;
// d_srcA == RNONE: 0;
// d_srcA == e_dstE : e_valE;   # value computed by ExecuteStage
// d_srcA == M_dstM: m_valM;  # value obtained from Memory by MemoryStage
// d_srcA == M_dstE: M_valE;  # value in M register
// d_srcA == W_dstM: W_valM;  # value in W register
// d_srcA == W_dstE : W_valE; # value in W register
// 1 : d_rvalA;  # value from register file
// ];

//tell this method how to grab variabels from other classes (M_dstE, e_valE)
uint64_t DecodeStage::fwdA(uint64_t d_srcA, uint64_t d_rvalA, M * mreg, W * wreg, uint64_t e_dstE, uint64_t e_valE, 
                                          uint64_t D_valP, uint64_t D_icode, uint64_t m_valM)
{

   //You need to pass pointers to the M and W registers to the forwarding methods and access them out of those registers

   uint64_t M_dstE = mreg -> getdstE() -> getOutput();
   uint64_t M_dstM = mreg -> getdstM() -> getOutput();
   uint64_t M_valE = mreg -> getvalE() -> getOutput();
   uint64_t W_dstE = wreg -> getdstE() -> getOutput();
   uint64_t W_dstM = wreg -> getdstM() -> getOutput();
   uint64_t W_valE = wreg -> getvalE() -> getOutput();
   uint64_t W_valM = wreg -> getvalM() -> getOutput();

   
   if (D_icode == ICALL || D_icode == IJXX) return D_valP;
   if (d_srcA == RNONE) return 0;
   if (d_srcA == e_dstE) return e_valE;  // value computed by ExecuteStage
   if (d_srcA == M_dstM) return m_valM; //value obtained from Memory by MemoryStage
   if (d_srcA == M_dstE) return M_valE; //value in M register
   if (d_srcA == W_dstM) return W_valM; // value in W register
   if (d_srcA == W_dstE) return W_valE; // value in W register
   else return d_rvalA;
   
   
}

uint64_t DecodeStage::fwdB(uint64_t d_srcB, uint64_t d_rvalB, M * mreg, W * wreg, 
                                                                  uint64_t e_dstE, uint64_t e_valE, uint64_t m_valM)
{
   //prevent this method from selecting a valE value that it doesn't actually use
   if (d_srcB == RNONE) return 0;
   //You need to pass pointers to the M and W registers to the forwarding methods and access them out of those registers
   uint64_t M_dstE = mreg -> getdstE() -> getOutput();
   uint64_t M_dstM = mreg -> getdstM() -> getOutput();
   uint64_t M_valE = mreg -> getvalE() -> getOutput();
   uint64_t W_dstE = wreg -> getdstE() -> getOutput();
   uint64_t W_valE = wreg -> getvalE() -> getOutput();
   uint64_t W_valM = wreg -> getvalM() -> getOutput();
   uint64_t W_dstM = wreg -> getdstM() -> getOutput();

   //printf("mvale %lx, w_vale %lx, d_rvalb %lx\n", M_valE, W_valE, d_rvalB);

   if (d_srcB == e_dstE) return e_valE; // value computed by ExecuteStage
   if (d_srcB == M_dstM) return m_valM; // value obtained from Memory by MemoryStage
   if (d_srcB == M_dstE) return M_valE; // value in M register
   if (d_srcB == W_dstM) return W_valM; // value in W register 
   if (d_srcB == W_dstE) return W_valE; // value in W register
   else return d_rvalB; 
}

void DecodeStage::calculateControlSignals(uint64_t E_icode, uint64_t E_dstM, uint64_t E_cnd){

   E_bubble = ((E_icode == IJXX && !E_cnd) || ((E_icode == IMRMOVQ || E_icode == IPOPQ) && (E_dstM == d_srcA || E_dstM == d_srcB)));
}

uint64_t DecodeStage::getsrcA(){
    return d_srcA;
}

uint64_t DecodeStage::getsrcB(){
    return d_srcB;
}


