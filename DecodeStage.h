
class DecodeStage : public Stage 
{
	public:
    		bool doClockLow(PipeReg **pregs, Stage **stages);
    		void doClockHigh(PipeReg **pregs);
			
			void setEinput(E * ereg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t valC, uint64_t valA, 
						   uint64_t valB, uint64_t dstE, uint64_t dstM, uint64_t srcA, uint64_t srcB);
			uint64_t getD_valA(uint64_t d_srcA, uint64_t d_rvalA, M * mreg, W * wreg, ExecuteStage * e);
			uint64_t getD_valB(uint64_t d_srcB, uint64_t d_rvalB, M * mreg, W * wreg, ExecuteStage * e );

};
