class ExecuteStage : public Stage
{
        private: 
                uint64_t E_dstE, E_valE;
                void ccLogicCircuit(uint64_t E_icode, uint64_t aluA, uint64_t aluB, uint64_t aluFun);
                bool setCC(uint64_t E_icode);
                void setMinput(M * mreg, uint64_t stat, uint64_t icode, 
                uint64_t Cnd, uint64_t valE, uint64_t valA, uint64_t dstE, uint64_t dstM);
                uint64_t getAluA(uint64_t E_icode, uint64_t E_valA, uint64_t);
                uint64_t getAluB(uint64_t E_icode, uint64_t E_valB, uint64_t);
                uint64_t getAluFun(uint64_t E_icode, uint64_t E_ifun);

        public:
                bool doClockLow(PipeReg **pregs, Stage **stages);
                void doClockHigh(PipeReg **pregs);
                uint64_t getDstE(uint64_t E_icode, uint64_t e_Cnd, uint64_t E_dstE);
                uint64_t getE_valE();
                uint64_t getE_dstE();
};

