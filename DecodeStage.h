
class DecodeStage : public Stage 
{
	public:
    		bool doClockLow(PipeReg **pregs, Stage **stages);
    		void doClockHigh(PipeReg **pregs);
			
			void setEinput(E * ereg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t valC, uint64_t valA, 
						   uint64_t valB, uint64_t dstE, uint64_t dstM, uint64_t srcA, uint64_t srcB);
		uint64_t buildsrcA(uint64_t D_icode, uint64_t rA);
		uint64_t buildsrcB(uint64_t D_icode, uint64_t rB);
		uint64_t builddstE(uint64_t D_icode, uint64_t rB);
		uint64_t builddstM(uint64_t D_icode, uint64_t rA);
		uint64_t buildvalA(uint64_t valA, uint64_t srcA);
		uint64_t buildvalB(uint64_t valB, uint64_t srcB);
		uint64_t fwdA(uint64_t d_srcA, uint64_t d_rvalA, M * mreg, W * wreg, uint64_t e_dstE, uint64_t e_valE, uint64_t D_valP, uint64_t D_icode, uint64_t m_valM);
		uint64_t fwdB(uint64_t d_srcB, uint64_t d_rvalB, M * mreg, W * wreg, uint64_t e_dstE, uint64_t e_valE, uint64_t m_valM);



};
