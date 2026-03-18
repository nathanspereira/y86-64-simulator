# Y86-64 Pipelined Processor Simulator

## Project Overview
This project is a functional simulator for a **Y86-64 pipelined processor**, written in **C**. It simulates the standard five-stage pipeline architecture, handling instruction fetching, decoding, execution, memory access, and write-back.

## Features
* **Full Pipeline Implementation:** Includes Fetch, Decode, Execute, Memory, and Write-back stages.
* **Hazard Handling:** Implements logic for data forwarding and pipeline stalling/bubbling to handle control and data hazards.
* **Instruction Support:** Supports standard Y86-64 instructions including `irmovq`, `rmmovq`, `mrmovq`, `OPq`, `jXX`, `call`, `ret`, `pushq`, and `popq`.
* **Condition Codes:** Functional Zero (ZF), Sign (SF), and Overflow (OF) flags for conditional jumps and moves.

## Technical Details
The simulator is structured into modular components representing the pipeline registers and stage logic:
* `FetchStage.C`: Handles instruction retrieval and PC prediction.
* `DecodeStage.C`: Manages register file access and hazard detection.
* `ExecuteStage.C`: Performs ALU operations and sets condition codes.
* `MemoryStage.C`: Interfaces with the simulated system memory.
* `WritebackStage.C`: Updates the architectural state.

## How to Run
1. **Compile the project:**
   ```bash
   make
    ```

2. Run a program:
   ```./yess <program_name>.yo```
