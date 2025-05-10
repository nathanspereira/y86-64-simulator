//class to perform the combinational logic of
//the Fetch stage
class FetchStage: public Stage
{
   private:
      bool F_stall;
      bool D_stall;
      bool D_bubble;
      void setDInput(D * dreg, uint64_t stat, uint64_t icode, uint64_t ifun, 
                     uint64_t rA, uint64_t rB,
                     uint64_t valC, uint64_t valP);
      bool instr_valid(uint64_t f_icode);
      void setStat(bool mem_error, bool instr_valid, uint64_t f_icode, uint64_t &stat);
      void calculateControlSignals(E *ereg, Stage **stages, D *dreg, M *mreg);
      void normalD(D *dreg);
      void bubbleD(D *dreg);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      uint64_t selectPC(F * freg, M * mreg, W * wreg);
      bool needRegIds(uint64_t f_icode);
      uint64_t predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP);
      bool needValC(uint64_t f_icode);
      uint64_t PCincrement(uint64_t f_pc, bool regldsBool, bool valCbool);
      void getRegIds(uint64_t f_icode, uint64_t pc, uint64_t & rA, uint64_t &rB );
      uint64_t buildValC(uint64_t f_pc, bool needRegBool, bool need_valC);

};
