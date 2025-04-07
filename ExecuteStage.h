class ExecuteStage : public Stage
{
        public:
                bool doClockLow(PipeReg **pregs, Stage **stages);
                void doClockHigh(PipeReg **pregs);

                void setMinput(M * mreg, uint64_t stat, uint64_t icode, 
                           uint64_t Cnd, uint64_t valE, uint64_t valA, uint64_t dstE, uint64_t dstM);
                
};

