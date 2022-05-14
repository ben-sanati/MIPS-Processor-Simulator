#include <iostream>
#include <string>

#include "Translator.h"
#include "Memory.h"
#include "PC.h"
#include "Control.h"
#include "InstructionRegister.h"
#include "ALU.h"
#include "MemoryDataRegister.h"
#include "ALUOut.h"
#include "RegisterFile.h"
#include "tempRegisterA.h"
#include "tempRegisterB.h"
#include "TestBench.h"

using namespace std;

void runIF(Memory* MemoryFile, Control* signals, InstructionRegister* updateTemp, MemoryDataRegister* placeTemp, ALU* arithLogUnit, PC* counter, ALUOut* arithLogOut, int* currentAddress, int* run, bool immSign, int* nextAddress, string* instructRegVal, string* opCode, string* rs, string* rt, string* imm, int* ALUresult, int* ALUzero) {
    signals->instructionFetch();                                                     //get IF control signals
    //Pass address value to instruction temporary register
    //IR=instruction @ PC address
    *instructRegVal = MemoryFile->returnAddressInstruction(signals->returnIorD(), *currentAddress, 0, 0, signals->returnMemRead(), signals->returnMemWrite()); //define memory file address
    updateTemp->updateRegister(*instructRegVal);                                      //update temporary instruction register
    placeTemp->inputInstruction(*instructRegVal);                                     //update temporary Memory register
    //PC instructions are now in registers, get the values from instruction register
    *opCode = updateTemp->returnOpCode();
    *rs = updateTemp->returnRs();
    *rt = updateTemp->returnRt();
    *imm = updateTemp->returnImm();

    //Pass PC value (currentAddress) to ALU
    arithLogUnit->aluInputs(*currentAddress, 0, 0, signals->returnALUSrcA(), signals->returnALUSrcB(), *imm);
    arithLogUnit->aluResultVal(signals->returnALUOp(), immSign);                               //get ALU arithmetic result
    *ALUresult = arithLogUnit->returnALUResult();
    *ALUzero = arithLogUnit->returnALUzero();
    arithLogOut->inputALUResult(*ALUresult);                                         //update temporary ALUOut register
    //ALU result and zero flag acquired

    //Pass ALUresult to PC
    //PC = PC + 4
    counter->updatePC(*ALUresult, 0, *imm, signals->returnPCSource(), signals->returnPCWrite(), signals->returnPCWriteCondition(), *ALUzero);
    *nextAddress = counter->returnCurrentAddress();                                //PC = PC + 4

    //Proof Working       ------------------------------------------------------------------
    //cout << "PC: " << *currentAddress << endl;
    //cout << "IR: " << *opCode + *rs + *rt + *imm << endl;
                          //------------------------------------------------------------------
}

void runID(Memory* MemoryFile, Control* signals, InstructionRegister* updateTemp, MemoryDataRegister* placeTemp, ALU* arithLogUnit, PC* counter, ALUOut* arithLogOut, RegisterFile* regFile, tempRegisterA* regA, tempRegisterB* regB, int* readReg1, int* readReg2, int* currentAddress, int* run, int reg[], bool immSign, string* instructRegVal, string* opCode, string* rs, string* rt, string* imm, string* operation, int* ALUresult, int* ALUzero) {
    signals->instructionDecode();                                                     //get ID control signals
    //pass opcode to control for decoding (this returns a string letting us know what the operation is e.g. beq)
    *operation = signals->instructionDecodeOperation(*opCode, *imm);

    //pass input data to register file
    regFile->regOperations(*rs, *rt, *imm, placeTemp->returnInstruction(), arithLogOut->returnALUResult(), signals->returnRegDst(), signals->returnMemtoReg(), signals->returnRegWrite());
    //get read reg positions
    *readReg1 = regFile->returnReadNumber1();
    *readReg2 = regFile->returnReadNumber2();

    //pass values to A and B        A=rs    B=rt
    regA->inputRsValue(reg[stoi(*rs, 0, 2)]);
    regB->inputRtValue(reg[stoi(*rt, 0, 2)]);

    //store ALU result in ALUOut
    //ALUOut = PC + (sign-extend(imm)<<2)
    arithLogUnit->aluInputs(*currentAddress, regA->returnAValue(), regB->returnBValue(), signals->returnALUSrcA(), signals->returnALUSrcB(), *imm);
    arithLogUnit->aluResultVal(signals->returnALUOp(), immSign);
    *ALUresult = arithLogUnit->returnALUResult();
    *ALUzero = arithLogUnit->returnALUzero();
    arithLogOut->inputALUResult(*ALUresult);

    //Proof Working       ------------------------------------------------------------------
    //cout << "A: " << regA->returnAValue() << " B: " << regB->returnBValue() << endl;
    //cout << "PC: " << *currentAddress << " ALUOut: " << arithLogOut->returnALUResult() << endl;
                          //------------------------------------------------------------------
}

//in cc1, PC=PC+4
//in cc2, ALUOut = PC + (sign-extend(imm)<<2)
void runBeqEXE(Memory* MemoryFile, Control* signals, InstructionRegister* updateTemp, MemoryDataRegister* placeTemp, ALU* arithLogUnit, PC* counter, ALUOut* arithLogOut, RegisterFile* regFile, tempRegisterA* regA, tempRegisterB* regB, int* currentAddress, int* run, bool immSign, int* nextAddress, string* instructRegVal, string* opCode, string* rs, string* rt, string* imm, string* operation, int* ALUresult, int* ALUzero) {
    signals->beqExecute();
    //pass values in regA and regB to ALU inputs
    arithLogUnit->aluInputs(*currentAddress, regA->returnAValue(), regB->returnBValue(), signals->returnALUSrcA(), signals->returnALUSrcB(), *imm);
    arithLogUnit->aluResultVal(signals->returnALUOp(), immSign);
    //get zero flag
    *ALUzero = arithLogUnit->returnALUzero();

    //if(A == B) ? PC = ALUout
    if((*ALUzero == 1) && (signals->returnPCWriteCondition() == 1)) {
        counter->updatePC(arithLogUnit->returnALUResult(), arithLogOut->returnALUResult(), *imm, signals->returnPCSource(), signals->returnPCWrite(), signals->returnPCWriteCondition(), arithLogUnit->returnALUzero());
        *nextAddress = counter->returnCurrentAddress();
    }
    //cout << "Next Address: " << *nextAddress << endl;

    //Proof Working       ------------------------------------------------------------------
    //cout << "Zero Flag: " << *ALUzero << endl;
    //cout << "PC: " << *nextAddress << endl;
                        //------------------------------------------------------------------
}

void runAddEXE(Memory* MemoryFile, Control* signals, InstructionRegister* updateTemp, MemoryDataRegister* placeTemp, ALU* arithLogUnit, PC* counter, ALUOut* arithLogOut, RegisterFile* regFile, tempRegisterA* regA, tempRegisterB* regB, int* currentAddress, int* run, int reg[], bool immSign, int* nextAddress, string* instructRegVal, string* opCode, string* rs, string* rt, string* imm, string* operation, int* ALUresult, int* ALUzero) {
    signals->addExecute();
    //pass values in regA and regB to ALU inputs
    arithLogUnit->aluInputs(*currentAddress, regA->returnAValue(), regB->returnBValue(), signals->returnALUSrcA(), signals->returnALUSrcB(), *imm);
    arithLogUnit->aluResultVal(signals->returnALUOp(), immSign);
    //store ALU result in ALUOut
    //A+B=ALUOut
    *ALUresult = arithLogUnit->returnALUResult();
    arithLogOut->inputALUResult(*ALUresult);

    //Proof Working       ------------------------------------------------------------------
    //cout << "A: " << regA->returnAValue() << " B: " << regB->returnBValue() << endl;
    //cout << "ALUOut: " << arithLogOut->returnALUResult() << endl;
                        //------------------------------------------------------------------
}

void runAddWB(Memory* MemoryFile, Control* signals, InstructionRegister* updateTemp, MemoryDataRegister* placeTemp, ALU* arithLogUnit, PC* counter, ALUOut* arithLogOut, RegisterFile* regFile, tempRegisterA* regA, tempRegisterB* regB, int* currentAddress, int* run, int reg[], bool immSign, int* nextAddress, string* instructRegVal, string* opCode, string* rs, string* rt, string* imm, string* operation, int* ALUresult, int* ALUzero) {
    signals->addWriteBack();
    //pass rd to register file and write to it
    regFile->regOperations(*rs, *rt, *imm, placeTemp->returnInstruction(), arithLogOut->returnALUResult(), signals->returnRegDst(), signals->returnMemtoReg(), signals->returnRegWrite());
    string rd = *imm;
    rd = rd.substr(0, 5);
    reg[stoi(rd, 0, 2)] = regFile->returnReg();

    //Proof Working       ------------------------------------------------------------------
    //string immTemp = *imm;
    //cout << "Rs: " << *rs << " Rt: " << *rt << " Rd: " << immTemp.substr(0, 5) << endl;
    //for(int i = 0; i < 32; i++)
        //cout << "$" << i << ": " << reg[i] << endl;
                        //------------------------------------------------------------------
}

void runJrEXE(Memory* MemoryFile, Control* signals, InstructionRegister* updateTemp, MemoryDataRegister* placeTemp, ALU* arithLogUnit, PC* counter, ALUOut* arithLogOut, RegisterFile* regFile, tempRegisterA* regA, tempRegisterB* regB, int* currentAddress, int* run, bool immSign, int* nextAddress, string* instructRegVal, string* opCode, string* rs, string* rt, string* imm, string* operation, int* ALUresult, int* ALUzero) {
    signals->jrExecute();
    //pass values in regA and regB to ALU inputs
    arithLogUnit->aluInputs(*currentAddress, regA->returnAValue(), regB->returnBValue(), signals->returnALUSrcA(), signals->returnALUSrcB(), *imm);
    arithLogUnit->aluResultVal(signals->returnALUOp(), immSign);
    //store ALU result in ALUOut
    //A+B=ALUOut
    *ALUresult = arithLogUnit->returnALUResult();
    arithLogOut->inputALUResult(*ALUresult);

    //Proof Working       ------------------------------------------------------------------
    //cout << "A: " << regA->returnAValue() << " B: " << regB->returnBValue() << endl;
    //cout << "ALUOut: " << arithLogOut->returnALUResult() << endl;
                        //------------------------------------------------------------------
}

void runJrWB(Memory* MemoryFile, Control* signals, InstructionRegister* updateTemp, MemoryDataRegister* placeTemp, ALU* arithLogUnit, PC* counter, ALUOut* arithLogOut, RegisterFile* regFile, tempRegisterA* regA, tempRegisterB* regB, int* currentAddress, int* run, bool immSign, int* nextAddress, string* instructRegVal, string* opCode, string* rs, string* rt, string* imm, string* operation, int* ALUresult, int* ALUzero) {
    signals->jrWriteBack();
    //pass ALUOut to PC
    counter->updatePC(arithLogUnit->returnALUResult(), arithLogOut->returnALUResult(), *imm, signals->returnPCSource(), signals->returnPCWrite(), signals->returnPCWriteCondition(), arithLogUnit->returnALUzero());
    *nextAddress = counter->returnCurrentAddress();

    //Proof Working       ------------------------------------------------------------------
    //cout << "ALUOut: " << arithLogOut->returnALUResult() << endl;
    //cout << "PC: " << *nextAddress << endl;
    //------------------------------------------------------------------
}

void runJEXE(Memory* MemoryFile, Control* signals, InstructionRegister* updateTemp, MemoryDataRegister* placeTemp, ALU* arithLogUnit, PC* counter, ALUOut* arithLogOut, RegisterFile* regFile, tempRegisterA* regA, tempRegisterB* regB, int* currentAddress, int* run, bool immSign, int* nextAddress, string* instructRegVal, string* opCode, string* rs, string* rt, string* imm, string* operation, int* ALUresult, int* ALUzero) {
    signals->jJalExecute();
    //shift imm left by 2 (*4) and add PC top 3 bits to value
    //PC=PC[31-28] | IR[25-0] << 2
    counter->updatePC(arithLogUnit->returnALUResult(), arithLogOut->returnALUResult(), *imm, signals->returnPCSource(), signals->returnPCWrite(), signals->returnPCWriteCondition(), arithLogUnit->returnALUzero());
    *nextAddress = counter->returnCurrentAddress();

    //Proof Working       ------------------------------------------------------------------
    //cout << "PC: " << *nextAddress << endl;
                        //------------------------------------------------------------------
}

void runSwLwAddiEXE(Memory* MemoryFile, Control* signals, InstructionRegister* updateTemp, MemoryDataRegister* placeTemp, ALU* arithLogUnit, PC* counter, ALUOut* arithLogOut, RegisterFile* regFile, tempRegisterA* regA, tempRegisterB* regB, int* currentAddress, int* run, bool immSign, int* nextAddress, string* instructRegVal, string* opCode, string* rs, string* rt, string* imm, string* operation, int* ALUresult, int* ALUzero) {
    signals->swLwAddiExecute();
    //pass imm to ALU input B
    //ALUOut = A + sign-extend(imm)
    arithLogUnit->aluInputs(*currentAddress, regA->returnAValue(), regB->returnBValue(), signals->returnALUSrcA(), signals->returnALUSrcB(), *imm);
    arithLogUnit->aluResultVal(signals->returnALUOp(), immSign);
    *ALUresult = arithLogUnit->returnALUResult();
    arithLogOut->inputALUResult(*ALUresult);
    //Proof Working       ------------------------------------------------------------------
    //cout << "A: " << regA->returnAValue() << " Imm: " << *imm << endl;
    //cout << "ALUOut: " << arithLogOut->returnALUResult() << endl;
                        //------------------------------------------------------------------
}

void runSwMA(Memory* MemoryFile, Control* signals, InstructionRegister* updateTemp, MemoryDataRegister* placeTemp, ALU* arithLogUnit, PC* counter, ALUOut* arithLogOut, RegisterFile* regFile, tempRegisterA* regA, tempRegisterB* regB, int* currentAddress, int* run, bool immSign, int* nextAddress, string* instructRegVal, string* opCode, string* rs, string* rt, string* imm, string* operation, string memoryFiler[] , int* ALUresult, int* ALUzero) {
    signals->swMemoryAccess();
    //memory[ALUOut] = B
    MemoryFile->returnAddressInstruction(signals->returnIorD(), *currentAddress, arithLogOut->returnALUResult(), regB->returnBValue(), signals->returnMemRead(), signals->returnMemWrite());
    memoryFiler[arithLogOut->returnALUResult()] = MemoryFile->returnMemData().substr(0, 8);
    memoryFiler[arithLogOut->returnALUResult() + 1] = MemoryFile->returnMemData().substr(8, 8);
    memoryFiler[arithLogOut->returnALUResult() + 2] = MemoryFile->returnMemData().substr(16, 8);
    memoryFiler[arithLogOut->returnALUResult() + 3] = MemoryFile->returnMemData().substr(24,  8);

    //Proof Working       ------------------------------------------------------------------
    //string immTemp = *imm;
    //cout << "Rs: " << *rs << " Rt: " << *rt << endl;
    //cout << "ALUOut: " << arithLogOut->returnALUResult() << " regB: " << regB->returnBValue() << endl;
    //cout << "Memory[" << arithLogOut->returnALUResult() << "] = " << MemoryFile->returnMemData() << endl;
                        //------------------------------------------------------------------
}

void runLwMA(Memory* MemoryFile, Control* signals, InstructionRegister* updateTemp, MemoryDataRegister* placeTemp, ALU* arithLogUnit, PC* counter, ALUOut* arithLogOut, RegisterFile* regFile, tempRegisterA* regA, tempRegisterB* regB, int* currentAddress, int* run, bool immSign, int* nextAddress, string* instructRegVal, string* opCode, string* rs, string* rt, string* imm, string* operation, string memoryFiler[] , int* ALUresult, int* ALUzero) {
    signals->lwMemoryAccess();
    //pass ALUOut to memory address
    //MDR = memory[ALUout]
    placeTemp->inputInstruction(memoryFiler[arithLogOut->returnALUResult()] + memoryFiler[arithLogOut->returnALUResult() + 1] + memoryFiler[arithLogOut->returnALUResult() + 2] + memoryFiler[arithLogOut->returnALUResult() + 3]);

    //Proof Working       ------------------------------------------------------------------
    //cout << "ALUOut: " << arithLogOut->returnALUResult() << " memory[ALUOut]: " << memoryFiler[arithLogOut->returnALUResult()] << endl;
    //cout << "MDR: " << placeTemp->returnInstruction() << endl;
                        //------------------------------------------------------------------
}

void runLwWB(Memory* MemoryFile, Control* signals, InstructionRegister* updateTemp, MemoryDataRegister* placeTemp, ALU* arithLogUnit, PC* counter, ALUOut* arithLogOut, RegisterFile* regFile, tempRegisterA* regA, tempRegisterB* regB, int* currentAddress, int* run, bool immSign, int* nextAddress, string* instructRegVal, string* opCode, string* rs, string* rt, string* imm, int reg[], string* operation, string memoryFiler[] , int* ALUresult, int* ALUzero) {
    signals->lwWriteBack();
    //pass value in MDR to registers at position rt
    //reg[rt] = MDR
    regFile->regOperations(*rs, *rt, *imm, placeTemp->returnInstruction(), arithLogOut->returnALUResult(), signals->returnRegDst(), signals->returnMemtoReg(), signals->returnRegWrite());
    string rtTemp = *rt;
    reg[stoi(rtTemp, 0, 2)] = regFile->returnReg();

    //Proof Working       ------------------------------------------------------------------
    //cout << "Rt: " << stoi(*rt, 0, 2) << " MDR: " << placeTemp->returnInstruction() << endl;
    //for(int i = 0; i < 32; i++)
        //cout << "$" << i << ": " << reg[i] << endl;
                        //------------------------------------------------------------------
}

void runAddiWB(Memory* MemoryFile, Control* signals, InstructionRegister* updateTemp, MemoryDataRegister* placeTemp, ALU* arithLogUnit, PC* counter, ALUOut* arithLogOut, RegisterFile* regFile, tempRegisterA* regA, tempRegisterB* regB, int* currentAddress, int* run, int reg[], bool immSign, int* nextAddress, string* instructRegVal, string* opCode, string* rs, string* rt, string* imm, string* operation, string memoryFiler[] , int* ALUresult, int* ALUzero) {
    signals->addiWBWriteBack();
    //reg[rt] = ALUOut
    regFile->regOperations(*rs, *rt, *imm, placeTemp->returnInstruction(), arithLogOut->returnALUResult(), signals->returnRegDst(), signals->returnMemtoReg(), signals->returnRegWrite());
    string rtTemp = *rt;
    reg[stoi(rtTemp, 0, 2)] = regFile->returnReg();

    //Proof Working       ------------------------------------------------------------------
    //cout << "Rt: " << stoi(*rt, 0, 2) << " ALUOut: " << arithLogOut->returnALUResult() << endl;
    //for(int i = 0; i < 32; i++)
        //cout << "$" << i << ": " << reg[i] << endl;
                        //------------------------------------------------------------------
}

void runInstructions(string instructions[1000], int* finalAddress, int* run, bool immSignArray[]) {
    //define objects
    Memory MemoryFile;
    PC counter(*finalAddress);
    Control signals;
    InstructionRegister updateTemp;
    MemoryDataRegister placeTemp;
    ALUOut arithLogOut;
    RegisterFile regFile(*finalAddress);
    tempRegisterA regA{};
    tempRegisterB regB{};
    TestBench testing;

    //setup variables
    int currentAddress;
    bool* addressLabel;
    bool tempBool;
    int nextAddress = 0;
    string instructRegVal, operation;
    string opCode, rs, rt, imm;
    int ALUresult, ALUzero;
    string memoryFiler[1000];
    int readReg1, readReg2;
    int numInstructions = 0;
    int ccErrors = 0;

    //setup register file array
    int reg[32];
    for(int i = 0; i < 32; i++)
        reg[i] = 0;

    //setup PC
    int clockCycle = 0;
    int instructionClockCycle = 0;
    currentAddress = counter.returnCurrentAddress();
    //PC is now setup

    //store instructions in memory
    for(int j = 0; j < *finalAddress / 4; j++) {
        MemoryFile.instructionstoStack(instructions[j], j);
    }
    *finalAddress = MemoryFile.updateFinalAddress();
    addressLabel = MemoryFile.returnAddressLabel();

    reg[29] = *finalAddress;               //value of $sp

    while(currentAddress <= *finalAddress) {
        if(*run == 0) {                                                     //clock mode
            tempBool = *(addressLabel + (currentAddress / 4));
            if(!tempBool) {
                //ALU uses comb logic therefore should be reset after each clock cycle
                ALU arithLogUnit;

                //starting IF (cycle 1)
                clockCycle++;
                instructionClockCycle++;
                cout << "Clock cycle: " << clockCycle << endl;
                cout << "Instruction Clock Cycle: " << instructionClockCycle << endl;
                runIF(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit, &counter, &arithLogOut, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &ALUresult, &ALUzero);

                for(int i = 0; i < 32; i++)
                    cout << "$" << i << ": " << reg[i] << endl;
                if(*run == 0)
                    cin.ignore();                                                                       //run through program clock cycle by clock cycle
                //end of IF (cycle 1)

                //ALU uses comb logic therefore should be reset after each clock cycle
                ALU arithLogUnit1;

                //starting ID cycle (cycle 2)
                clockCycle++;
                instructionClockCycle++;
                cout << "Clock cycle: " << clockCycle << endl;
                cout << "Instruction Clock Cycle: " << instructionClockCycle << endl;
                runID(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit1, &counter, &arithLogOut, &regFile, &regA, &regB, &readReg1, &readReg2, &currentAddress, run, reg, immSignArray[currentAddress / 4], &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);

                for(int i = 0; i < 32; i++)
                    cout << "$" << i << ": " << reg[i] << endl;
                if(*run == 0)
                    cin.ignore();                                                                       //run through program clock cycle by clock cycle
                //end of ID (cycle 2)

                //from here, each clock cycle is instruction specific
                //ALU uses comb logic therefore should be reset after each clock cycle
                ALU arithLogUnit2;

                if(operation == "beq") {                                                    //3 total clock cycles
                    //starting beq EXE cycle (cycle 3)
                    clockCycle++;
                    instructionClockCycle++;
                    cout << "Clock cycle: " << clockCycle << endl;
                    cout << "Instruction Clock Cycle: " << instructionClockCycle << endl;
                    runBeqEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);


                    for(int i = 0; i < 32; i++)
                        cout << "$" << i << ": " << reg[i] << endl;
                    if(*run == 0)
                        cin.ignore();                                                                       //run through program clock cycle by clock cycle
                    //end of beq EXE cycle (cycle 3)
                } else if(operation == "add") {                                        //3 total clock cycles
                    //starting add EXE cycle (cycle 3)                                          //4 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    cout << "Clock cycle: " << clockCycle << endl;
                    cout << "Instruction Clock Cycle: " << instructionClockCycle << endl;
                    runAddEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, reg, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);

                    for(int i = 0; i < 32; i++)
                        cout << "$" << i << ": " << reg[i] << endl;
                    if(*run == 0)
                        cin.ignore();                                                                       //run through program clock cycle by clock cycle
                    //end of add EXE cycle (cycle 3)

                    //starting add WB cycle (cycle 4)                                          //4 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    cout << "Clock cycle: " << clockCycle << endl;
                    cout << "Instruction Clock Cycle: " << instructionClockCycle << endl;
                    runAddWB(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, reg, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);

                    for(int i = 0; i < 32; i++)
                        cout << "$" << i << ": " << reg[i] << endl;
                    if(*run == 0)
                        cin.ignore();                                                                       //run through program clock cycle by clock cycle
                    //end of add WB cycle (cycle 4)
                } else if(operation == "jr") {
                    //starting jr EXE cycle (cycle 3)                                          //4 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    cout << "Clock cycle: " << clockCycle << endl;
                    cout << "Instruction Clock Cycle: " << instructionClockCycle << endl;
                    runJrEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);

                    for(int i = 0; i < 32; i++)
                        cout << "$" << i << ": " << reg[i] << endl;
                    if(*run == 0)
                        cin.ignore();                                                                       //run through program clock cycle by clock cycle
                    //end of jr EXE cycle (cycle 3)

                    //starting jr WB cycle (cycle 4)                                          //4 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    cout << "Clock cycle: " << clockCycle << endl;
                    cout << "Instruction Clock Cycle: " << instructionClockCycle << endl;
                    runJrWB(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);

                    for(int i = 0; i < 32; i++)
                        cout << "$" << i << ": " << reg[i] << endl;
                    if(*run == 0)
                        cin.ignore();                                                                       //run through program clock cycle by clock cycle
                    //end of jr WB cycle (cycle 4)

                    //reset value of $ra
                    reg[31] = 0;
                } else if((operation == "j") || (operation == "jal")) {
                    //starting j/jal EXE cycle (cycle 3)                                          //3 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    cout << "Clock cycle: " << clockCycle << endl;
                    cout << "Instruction Clock Cycle: " << instructionClockCycle << endl;
                    runJEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //set $ra to next address
                    if(operation == "jal")
                        reg[31] = arithLogOut.returnALUResult();

                    for(int i = 0; i < 32; i++)
                        cout << "$" << i << ": " << reg[i] << endl;
                    if(*run == 0)
                        cin.ignore();                                                                       //run through program clock cycle by clock cycle
                    //end of j/jal EXE cycle (cycle 3)
                } else if((operation == "sw") || (operation == "lw") || (operation == "addi")){
                    //starting sw/lw/addi EXE cycle (cycle 3)                                          //4/5 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    cout << "Clock cycle: " << clockCycle << endl;
                    cout << "Instruction Clock Cycle: " << instructionClockCycle << endl;
                    runSwLwAddiEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);

                    for(int i = 0; i < 32; i++)
                        cout << "$" << i << ": " << reg[i] << endl;
                    if(*run == 0)
                        cin.ignore();                                                                       //run through program clock cycle by clock cycle
                    //end of sw/lw/addi EXE cycle (cycle 3)

                    if(operation == "sw") {
                        //starting sw MA cycle (cycle 4)                                          //4 total clock cycles
                        clockCycle++;
                        instructionClockCycle++;
                        cout << "Clock cycle: " << clockCycle << endl;
                        cout << "Instruction Clock Cycle: " << instructionClockCycle << endl;
                        runSwMA(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, memoryFiler, &ALUresult, &ALUzero);

                        for(int i = 0; i < 32; i++)
                            cout << "$" << i << ": " << reg[i] << endl;
                        if(*run == 0)
                            cin.ignore();                                                                       //run through program clock cycle by clock cycle
                        //end of sw MA cycle (cycle 4)
                    } else if(operation == "lw") {
                        //starting lw MA cycle (cycle 4)                                          //5 total clock cycles
                        clockCycle++;
                        instructionClockCycle++;
                        cout << "Clock cycle: " << clockCycle << endl;
                        cout << "Instruction Clock Cycle: " << instructionClockCycle << endl;
                        runLwMA(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, memoryFiler, &ALUresult, &ALUzero);

                        for(int i = 0; i < 32; i++)
                            cout << "$" << i << ": " << reg[i] << endl;
                        if(*run == 0)
                            cin.ignore();                                                                       //run through program clock cycle by clock cycle
                        //end of lw MA cycle (cycle 4)

                        //starting lw WB cycle (cycle 5)                                          //5 total clock cycles
                        clockCycle++;
                        instructionClockCycle++;
                        cout << "Clock cycle: " << clockCycle << endl;
                        cout << "Instruction Clock Cycle: " << instructionClockCycle << endl;
                        runLwWB(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, reg, &operation, memoryFiler, &ALUresult, &ALUzero);

                        for(int i = 0; i < 32; i++)
                            cout << "$" << i << ": " << reg[i] << endl;
                        if(*run == 0)
                            cin.ignore();                                                                       //run through program clock cycle by clock cycle
                        //end of lw WB cycle (cycle 5)
                    } else if(operation == "addi") {
                        //starting addi WB cycle (cycle 4)                                          //4 total clock cycles
                        clockCycle++;
                        instructionClockCycle++;
                        cout << "Clock cycle: " << clockCycle << endl;
                        cout << "Instruction Clock Cycle: " << instructionClockCycle << endl;
                        runAddiWB(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, reg, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, memoryFiler, &ALUresult, &ALUzero);

                        for(int i = 0; i < 32; i++)
                            cout << "$" << i << ": " << reg[i] << endl;
                        if(*run == 0)
                            cin.ignore();                                                                       //run through program clock cycle by clock cycle
                        //end of addi WB cycle (cycle 4)
                    }
                }

                currentAddress = nextAddress;
                cout << "Command: " << operation << endl << endl;
            } else{
                //there was a label here and therefore we will skip it
                currentAddress += 4;
            }
        } else if(*run == 1) {                                              //run mode
            tempBool = *(addressLabel + (currentAddress / 4));
            if(!tempBool) {
                //ALU uses comb logic therefore should be reset after each clock cycle
                ALU arithLogUnit;

                //starting IF (cycle 1)
                clockCycle++;
                instructionClockCycle++;
                runIF(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit, &counter, &arithLogOut, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &ALUresult, &ALUzero);
                //end of IF (cycle 1)

                //ALU uses comb logic therefore should be reset after each clock cycle
                ALU arithLogUnit1;

                //starting ID cycle (cycle 2)
                clockCycle++;
                instructionClockCycle++;
                runID(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit1, &counter, &arithLogOut, &regFile, &regA, &regB, &readReg1, &readReg2, &currentAddress, run, reg, immSignArray[currentAddress / 4], &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                //end of ID (cycle 2)

                //from here, each clock cycle is instruction specific
                //ALU uses comb logic therefore should be reset after each clock cycle
                ALU arithLogUnit2;

                if(operation == "beq") {                                                    //3 total clock cycles
                    //starting beq EXE cycle (cycle 3)
                    clockCycle++;
                    instructionClockCycle++;
                    runBeqEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //end of beq EXE cycle (cycle 3)
                } else if(operation == "add") {                                        //3 total clock cycles
                    //starting add EXE cycle (cycle 3)                                          //4 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    runAddEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, reg, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //end of add EXE cycle (cycle 3)

                    //starting add WB cycle (cycle 4)                                          //4 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    runAddWB(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, reg, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //end of add WB cycle (cycle 4)
                } else if(operation == "jr") {
                    //starting jr EXE cycle (cycle 3)                                          //4 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    runJrEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //end of jr EXE cycle (cycle 3)

                    //starting jr WB cycle (cycle 4)                                          //4 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    runJrWB(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //end of jr WB cycle (cycle 4)

                    //reset value of $ra
                    reg[31] = 0;
                } else if((operation == "j") || (operation == "jal")) {
                    //starting j/jal EXE cycle (cycle 3)                                          //3 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    runJEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //set $ra to next address
                    if(operation == "jal")
                        reg[31] = arithLogOut.returnALUResult();
                    //end of j/jal EXE cycle (cycle 3)
                } else if((operation == "sw") || (operation == "lw") || (operation == "addi")){
                    //starting sw/lw/addi EXE cycle (cycle 3)                                          //4/5 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    runSwLwAddiEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //end of sw/lw/addi EXE cycle (cycle 3)

                    if(operation == "sw") {
                        //starting sw MA cycle (cycle 4)                                          //4 total clock cycles
                        clockCycle++;
                        instructionClockCycle++;
                        runSwMA(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, memoryFiler, &ALUresult, &ALUzero);
                        //end of sw MA cycle (cycle 4)
                    } else if(operation == "lw") {
                        //starting lw MA cycle (cycle 4)                                          //5 total clock cycles
                        clockCycle++;
                        instructionClockCycle++;
                        runLwMA(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, memoryFiler, &ALUresult, &ALUzero);
                        //end of lw MA cycle (cycle 4)

                        //starting lw WB cycle (cycle 5)                                          //5 total clock cycles
                        clockCycle++;
                        instructionClockCycle++;
                        runLwWB(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, reg, &operation, memoryFiler, &ALUresult, &ALUzero);
                        //end of lw WB cycle (cycle 5)
                    } else if(operation == "addi") {
                        //starting addi WB cycle (cycle 4)                                          //4 total clock cycles
                        clockCycle++;
                        instructionClockCycle++;
                        runAddiWB(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, reg, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, memoryFiler, &ALUresult, &ALUzero);
                        //end of addi WB cycle (cycle 4)
                    }
                }

                currentAddress = nextAddress;
            } else{
                //there was a label here and therefore we will skip it
                currentAddress += 4;
            }
        } else if(*run == 2) {                                                               //testbench mode
            tempBool = *(addressLabel + (currentAddress / 4));
            if(!tempBool) {
                //ALU uses comb logic therefore should be reset after each clock cycle
                ALU arithLogUnit;

                //starting IF (cycle 1)
                clockCycle++;
                instructionClockCycle++;
                runIF(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit, &counter, &arithLogOut, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &ALUresult, &ALUzero);
                //end of IF (cycle 1)

                //ALU uses comb logic therefore should be reset after each clock cycle
                ALU arithLogUnit1;

                //starting ID cycle (cycle 2)
                clockCycle++;
                instructionClockCycle++;
                runID(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit1, &counter, &arithLogOut, &regFile, &regA, &regB, &readReg1, &readReg2, &currentAddress, run, reg, immSignArray[currentAddress / 4], &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                //end of ID (cycle 2)

                //from here, each clock cycle is instruction specific
                //ALU uses comb logic therefore should be reset after each clock cycle
                ALU arithLogUnit2;

                if(operation == "beq") {                                                    //3 total clock cycles
                    //starting beq EXE cycle (cycle 3)
                    clockCycle++;
                    instructionClockCycle++;
                    runBeqEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //end of beq EXE cycle (cycle 3)

                    ccErrors += testing.ccErrorVal(operation, instructionClockCycle);
                } else if(operation == "add") {                                        //3 total clock cycles
                    //starting add EXE cycle (cycle 3)                                          //4 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    runAddEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, reg, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //end of add EXE cycle (cycle 3)

                    //starting add WB cycle (cycle 4)                                          //4 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    runAddWB(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, reg, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //end of add WB cycle (cycle 4)

                    ccErrors += testing.ccErrorVal(operation, instructionClockCycle);
                } else if(operation == "jr") {
                    //starting jr EXE cycle (cycle 3)                                          //4 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    runJrEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //end of jr EXE cycle (cycle 3)

                    //starting jr WB cycle (cycle 4)                                          //4 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    runJrWB(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //end of jr WB cycle (cycle 4)

                    //reset value of $ra
                    reg[31] = 0;

                    ccErrors += testing.ccErrorVal(operation, instructionClockCycle);
                } else if((operation == "j") || (operation == "jal")) {
                    //starting j/jal EXE cycle (cycle 3)                                          //3 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    runJEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //set $ra to next address
                    if(operation == "jal")
                        reg[31] = arithLogOut.returnALUResult();
                    //end of j/jal EXE cycle (cycle 3)

                    ccErrors += testing.ccErrorVal(operation, instructionClockCycle);
                } else if((operation == "sw") || (operation == "lw") || (operation == "addi")){
                    //starting sw/lw/addi EXE cycle (cycle 3)                                          //4/5 total clock cycles
                    clockCycle++;
                    instructionClockCycle++;
                    runSwLwAddiEXE(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, &ALUresult, &ALUzero);
                    //end of sw/lw/addi EXE cycle (cycle 3)

                    if(operation == "sw") {
                        //starting sw MA cycle (cycle 4)                                          //4 total clock cycles
                        clockCycle++;
                        instructionClockCycle++;
                        runSwMA(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, memoryFiler, &ALUresult, &ALUzero);
                        //end of sw MA cycle (cycle 4)

                        ccErrors += testing.ccErrorVal(operation, instructionClockCycle);
                    } else if(operation == "lw") {
                        //starting lw MA cycle (cycle 4)                                          //5 total clock cycles
                        clockCycle++;
                        instructionClockCycle++;
                        runLwMA(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, memoryFiler, &ALUresult, &ALUzero);
                        //end of lw MA cycle (cycle 4)

                        //starting lw WB cycle (cycle 5)                                          //5 total clock cycles
                        clockCycle++;
                        instructionClockCycle++;
                        runLwWB(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, reg, &operation, memoryFiler, &ALUresult, &ALUzero);
                        //end of lw WB cycle (cycle 5)

                        ccErrors += testing.ccErrorVal(operation, instructionClockCycle);
                    } else if(operation == "addi") {
                        //starting addi WB cycle (cycle 4)                                          //4 total clock cycles
                        clockCycle++;
                        instructionClockCycle++;
                        runAddiWB(&MemoryFile, &signals, &updateTemp, &placeTemp, &arithLogUnit2, &counter, &arithLogOut, &regFile, &regA, &regB, &currentAddress, run, reg, immSignArray[currentAddress / 4], &nextAddress, &instructRegVal, &opCode, &rs, &rt, &imm, &operation, memoryFiler, &ALUresult, &ALUzero);
                        //end of addi WB cycle (cycle 4)

                        ccErrors += testing.ccErrorVal(operation, instructionClockCycle);
                    }
                }

                currentAddress = nextAddress;
            } else{
                //there was a label here and therefore we will skip it
                currentAddress += 4;
            }
        }
        numInstructions++;
        instructionClockCycle = 0;
    }

    if((*run == 0) || (*run == 1)) {
        cout << "Memory Filer: " << endl;
        for(int i = 0; i < 1000; i++)
            cout << "Pos " << i << ": " << memoryFiler[i] << endl;

        cout << endl << endl << "Final Register values:" << endl;
        for(int i = 0; i < 32; i++)
            cout << "$" << i << ": " << reg[i] << endl;
    } else if(*run == 2) {
        cout << endl << endl << "------------------------------------------------------" << endl;
        cout << "Testing Mode:" << endl;
        cout << "------------------------------------------------------" << endl;
        cout << endl << "Total Number of Clock Cycles: " << clockCycle << endl;
        cout << "Number of Clock Cycle Errors: " << ccErrors << endl;
        cout << "Total Number of Instructions: " << numInstructions << endl;
    }
    cout << endl << "End of Program" << endl;
}

int main() {
    //receive translated code
    Translator mips_Assembly;
    bool* immSign;
    string instructions[1000];
    int op[1000];
    bool immSignArray[1000];

    //prints code from file to console
    //mips_Assembly.getFileInput();
    //get final address
    int finalAddress = mips_Assembly.returnFinalAddress();

    //get binary instructions and decimal opcodes
    for(int i = 0; i < finalAddress / 4; i++) {
        op[i] = 0;
        instructions[i] = mips_Assembly.returnBinaryInstruction(i);
        if(instructions[i].empty() == 0) {
            op[i] = mips_Assembly.returnDecimalOpcode(i);
        }
        //cout << i * 4 << ": " << instructions[i] << endl;
    }

    immSign = mips_Assembly.returnImmSign();                //immSign here is a ref
    for(int i = 0; i < finalAddress / 4; i++)
        immSignArray[i] = *(immSign + i);
    //translator code received

    //output menu:
    int run = 0;
    string temp;
    cout << endl << "Modes of Operation:" << endl;
    cout << "------------------------------------------------------" << endl;
    cout << "Cycle by Cycle (0)" << endl;
    cout << "Normal Run (1)" << endl;
    cout << "Testing mode (2)" << endl;
    cout << "------------------------------------------------------" << endl;
    cout << "Selection:";
    getline(cin, temp);
    run = stoi(temp);
    //menu outputted

    //store and run instructions
    runInstructions(instructions, &finalAddress, &run, immSignArray);

    return 0;
}
