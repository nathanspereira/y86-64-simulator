
class DecodeStage : public Stage 
{
	public:
    		bool doClockLow(PipeReg **pregs, Stage **stages);
    		void doClockHigh(PipeReg **pregs);
			
			void setEinput(E * ereg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t valC, uint64_t valA, 
						   uint64_t valB, uint64_t dstE, uint64_t dstM, uint64_t srcA, uint64_t srcB);
		uint64_t buildSrcA(uint64_t D_icode, uint64_t rA);
		uint64_t buildSrcB(uint64_t D_icode, uint64_t rB);
		uint64_t buildDstE(uint64_t D_icode, uint64_t rB);
		uint64_t buildDstM(uint64_t D_icode, uint64_t rA);
		uint64_t buildValA(uint64_t valA);
		uint64_t buildValB(uint64_t valB);
};
