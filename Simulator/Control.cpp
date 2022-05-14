#include "Control.h"

Control::Control() {
    PCWriteCond = 0;
    PCWrite = 0;
    IorD = 0;
    MemRead = 0;
    MemWrite = 0;
    MemtoReg = 0;
    IRWrite = 0;
    PCSource = 0;
    ALUOp = 0;
    ALUSrcA = 0;
    ALUSrcB = 0;
    RegWrite = 0;
    RegDst = 0;
}

//in cc1, IR = instruction @ PC address
//PC = PC + 4
void Control::instructionFetch() {
    PCWriteCond = 0;                                                            //initialize all values
    PCWrite = 0;
    IorD = 0;
    MemRead = 0;
    MemWrite = 0;
    MemtoReg = 0;
    IRWrite = 0;
    PCSource = 0;
    ALUOp = 0;
    ALUSrcA = 0;
    ALUSrcB = 0;
    RegWrite = 0;
    RegDst = 0;
    MemRead = 1;                                                                //read instruction in memory
    IRWrite = 1;                                                                //write to instruction register
    PCWrite = 1;                                                                //write to PC
    ALUSrcB = 1;                                                                //add 4 to PC
}

//in cc2, A = rs, B = rt
//ALUout = PC+sign-extend(imm)<<2 (*4)
void Control::instructionDecode() {
    PCWriteCond = 0;                                                            //initialize all values
    PCWrite = 0;
    IorD = 0;
    MemRead = 0;
    MemWrite = 0;
    MemtoReg = 0;
    IRWrite = 0;
    PCSource = 0;
    ALUOp = 0;
    ALUSrcA = 0;
    RegWrite = 0;
    RegDst = 0;
    //PCWrite = 0                                                                Don't write to PC
    //ALUSrcA = 0;                                                               Pass PC value to ALU
    ALUSrcB = 3;                                                                //Pass sign-extend(imm)<<2 to ALU
    //ALUOp = 0;                                                                  ALU add command
}

string Control::instructionDecodeOperation(string opCode, string imm) {
    //if opcode "XXXXXX" -> the command is "XXX"
    if(opCode == "000000") {
        //R type (need to get funct from imm)
        string funct;
        funct = imm.substr(10, 6);
        //cout << "funct: " << funct << endl;
        if(funct == "100000") {
            return "add";
        } else if (funct == "001000") {
            return "jr";
        }
    } else if(opCode == "001000") {
        //addi
        return "addi";
    } else if(opCode == "000100") {
        //beq
        return "beq";
    } else if(opCode == "101011") {
        //sw
        return "sw";
    } else if(opCode == "010111") {
        //lw
        return "lw";
    } else if(opCode == "000010") {
        //j
        return "j";
    } else if(opCode == "000011") {
        //jal
        return "jal";
    }
    return "unkown";
}

//beq Execute command
//branching only requires 3 cc
//in cc3, if(A==B), PC = ALUout
void Control::beqExecute() {
    PCWrite = 0;                                                               //initialize all values
    IorD = 0;
    MemRead = 0;
    MemWrite = 0;
    MemtoReg = 0;
    IRWrite = 0;
    ALUSrcB = 0;
    RegWrite = 0;
    RegDst = 0;
    ALUSrcA = 1;                                                                //input rs to ALU
    //ALUSrcB = 0                                                                 input rt to ALU
    ALUOp = 1;                                                                  //ALU subtract command
    PCWriteCond = 1;                                                            //if Zero flag is set, then equal and branch PC
    PCSource = 1;                                                               //pass ALUout value to PC
}

//add R type command
//adding requires 4 cc
//in cc3, A+B = ALUout
void Control::addExecute() {
    PCWriteCond = 0;                                                            //initialize all values
    PCWrite = 0;
    IorD = 0;
    MemRead = 0;
    MemWrite = 0;
    MemtoReg = 0;
    IRWrite = 0;
    PCSource = 0;
    ALUSrcB = 0;
    RegWrite = 0;
    RegDst = 0;
    ALUSrcA = 1;                                                                //input rs to ALU
    //ALUSrcB = 0                                                                 input rt to ALU
    ALUOp = 2;                                                                  //ALU function command
}
//in cc4, we must write the ALU result to the register file
void Control::addWriteBack() {
    PCWriteCond = 0;                                                            //initialize all values
    PCWrite = 0;
    IorD = 0;
    MemRead = 0;
    MemWrite = 0;
    MemtoReg = 0;
    IRWrite = 0;
    PCSource = 0;
    ALUOp = 0;
    ALUSrcA = 0;
    ALUSrcB = 0;
    RegDst = 1;                                                                 //pass rd to write register in reg file
    //MemtoReg = 0;                                                               pass ALUout value to write data in reg file
    RegWrite = 1;                                                               //Write to reg file
}

//jr R type command
//jr requires 4 cc
//in cc3, A+B=ALUout
void Control::jrExecute() {
    PCWriteCond = 0;                                                            //initialize all values
    PCWrite = 0;
    IorD = 0;
    MemRead = 0;
    MemWrite = 0;
    MemtoReg = 0;
    IRWrite = 0;
    PCSource = 0;
    ALUOp = 2;
    ALUSrcB = 0;
    RegWrite = 0;
    RegDst = 0;
    ALUSrcA = 1;                                                                //pass rs to ALU input
    //ALUSrcB = 0;                                                              //pass rt to ALU input
    //ALUOp = 0;                                                                  ALU add command
}

//PC = rs
void Control::jrWriteBack() {
    PCWriteCond = 0;                                                            //initialize all values
    IorD = 0;
    MemRead = 0;
    MemWrite = 0;
    MemtoReg = 0;
    IRWrite = 0;
    ALUOp = 0;
    ALUSrcA = 0;
    ALUSrcB = 0;
    RegWrite = 0;
    RegDst = 0;
    PCSource = 1;                                                               //pass ALUout to PC
    PCWrite = 1;                                                                //update PC
}

//j and jal J type command
//j and jal require 3 cc
//in cc3, PC = PC[31-28] + IR[25-0] << 2       (*4)
void Control::jJalExecute() {
    PCWriteCond = 0;                                                            //initialize all values
    IorD = 0;
    MemRead = 0;
    MemWrite = 0;
    MemtoReg = 0;
    IRWrite = 0;
    ALUOp = 0;
    ALUSrcA = 0;
    ALUSrcB = 0;
    RegWrite = 0;
    RegDst = 0;
    PCWrite = 1;                                                                //update PC
    PCSource = 2;                                                               //pass imm address to PC
}

//sw and lw I type command
//sw -> 4 cc and lw -> 5cc
//in cc3, ALUout = A + sign-extend(imm)
void Control::swLwAddiExecute() {
    PCWriteCond = 0;                                                            //initialize all values
    PCWrite = 0;
    IorD = 0;
    MemRead = 0;
    MemWrite = 0;
    MemtoReg = 0;
    IRWrite = 0;
    PCSource = 0;
    ALUOp = 0;
    RegWrite = 0;
    RegDst = 0;
    ALUSrcA = 1;
    ALUSrcB = 2;                                                                //pass sign extended immediate
    //ALUOp = 0;                                                                  ALU add command
}

//in sw cc4, memory[ALUout] = B
void Control::swMemoryAccess() {
    PCWriteCond = 0;                                                            //initialize all values
    PCWrite = 0;
    MemRead = 0;
    MemtoReg = 0;
    IRWrite = 0;
    PCSource = 0;
    ALUOp = 0;
    ALUSrcA = 0;
    ALUSrcB = 0;
    RegWrite = 0;
    RegDst = 0;
    MemWrite = 1;                                                               //write to memory
    IorD = 1;                                                                   //pass ALUout to memory Address
}

//in lw cc4, MDR=memory[ALUout]
void Control::lwMemoryAccess() {
    PCWriteCond = 0;                                                            //initialize all values
    PCWrite = 0;
    MemWrite = 0;
    MemtoReg = 0;
    IRWrite = 0;
    PCSource = 0;
    ALUOp = 0;
    ALUSrcA = 0;
    ALUSrcB = 0;
    RegWrite = 0;
    RegDst = 0;
    IorD = 1;                                                                   //pass ALUout to memory Address
    MemRead = 1;                                                                //read from memory
}

//in lw cc5, we write value to register destination
void Control::lwWriteBack() {
    PCWriteCond = 0;                                                            //initialize all values
    PCWrite = 0;
    IorD = 0;
    MemRead = 0;
    MemWrite = 0;
    IRWrite = 0;
    PCSource = 0;
    ALUOp = 0;
    ALUSrcA = 0;
    ALUSrcB = 0;
    RegWrite = 1;
    RegDst = 0;
    //RegDst = 0                                                                  pass rt to reg WriteRegister
    MemtoReg = 1;                                                               //pass value in MDR to reg writeData
}

void Control::addiWBWriteBack() {
    PCWriteCond = 0;                                                            //initialize all values
    PCWrite = 0;
    IorD = 0;
    MemRead = 0;
    MemWrite = 0;
    MemtoReg = 0;
    IRWrite = 0;
    PCSource = 0;
    ALUOp = 0;
    ALUSrcA = 0;
    ALUSrcB = 0;
    RegWrite = 1;
    RegDst = 0;
    //RegDst = 0;                                                                 pass rt to reg writeReg
    //MemtoReg = 0;                                                               pass ALUout to reg writeData
}

//return methods:

int Control::returnPCWriteCondition() {
    return PCWriteCond;
}

int Control::returnPCWrite() {
    return PCWrite;
}

int Control::returnIorD() {
    return IorD;
}

bool Control::returnMemRead() {
    return MemRead;
}

bool Control::returnMemWrite() {
    return MemWrite;
}

int Control::returnMemtoReg() {
    return MemtoReg;
}

bool Control::returnIRWrite() {
    return IRWrite;
}

int Control::returnPCSource() {
    return PCSource;
}

int Control::returnALUOp() {
    return ALUOp;
}

int Control::returnALUSrcA() {
    return ALUSrcA;
}

int Control::returnALUSrcB() {
    return ALUSrcB;
}

bool Control::returnRegWrite() {
    return RegWrite;
}

int Control::returnRegDst() {
    return RegDst;
}