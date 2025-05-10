class ExecuteStage : public Stage
{
        private: 
                bool M_bubble;
		uint64_t e_dstE, e_valE;
		uint64_t e_Cnd;
                void ccLogicCircuit(uint64_t E_icode, uint64_t aluA, uint64_t aluB, uint64_t aluFun);
                bool set_cc(uint64_t E_icode, uint64_t m_stat, uint64_t W_stat);
                void setMinput(M * mreg, uint64_t stat, uint64_t icode, 
                uint64_t Cnd, uint64_t valE, uint64_t valA, uint64_t dstE, uint64_t dstM);
                uint64_t getAluA(uint64_t E_icode, uint64_t E_valA, uint64_t E_valC);
                uint64_t getAluB(uint64_t E_icode, uint64_t E_valB);
                uint64_t getAluFun(uint64_t E_icode, uint64_t E_ifun);
                uint64_t aluLogicCircuit(uint64_t aluA, uint64_t aluB, uint64_t aluFun);
                void ccLogicCircuit(bool ccChanged, uint64_t aluA, uint64_t aluB,  uint64_t aluFun, uint64_t E_valE, bool &falsy);
                uint64_t set_dstE(uint64_t E_icode, uint64_t e_Cnd, uint64_t dstE);
                bool cond(uint64_t E_icode, uint64_t E_ifun);
		void bubbleM(M * mreg);
		void normalM(M * mreg);
		void calculateControlSignals(uint64_t m_stat, uint64_t W_stat);

        public:
                bool doClockLow(PipeReg **pregs, Stage **stages);
                void doClockHigh(PipeReg **pregs);
                uint64_t gete_valE();
                uint64_t gete_dstE();
		uint64_t gete_Cnd();

                
};

