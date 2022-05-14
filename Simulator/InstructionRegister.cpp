//
// Created by benja on 08/04/2021.
//

#include "InstructionRegister.h"

InstructionRegister::InstructionRegister() {
    //initialize all registers to 0
    opcode = "000000";
    rs = "00000";
    rt = "00000";
    imm = "0000000000000000";
}

void InstructionRegister::updateRegister(string instruction) {
    //split instruction up into opcode, rs etc.
    //cout << instruction << endl;
    opcode = instruction.substr(0, 6);
    rs = instruction.substr(6, 5);
    rt = instruction.substr(11, 5);
    imm = instruction.substr(16, 16);
    //cout << "Opcode: " << opcode << " Rs: " << rs << " Rt: " << rt << " Imm: " << imm << endl;
}

string InstructionRegister::returnOpCode() {
    return opcode;
}

string InstructionRegister::returnRs() {
    return rs;
}

string InstructionRegister::returnRt() {
    return rt;
}

string InstructionRegister::returnImm() {
    return imm;
}