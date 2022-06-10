//
// Created by benja on 09/04/2021.
//

#include "MemoryDataRegister.h"
void MemoryDataRegister::inputInstruction(string instruction) {
    instructionForNextCC = instruction;
    //cout << "Instruction: " << instruction << endl;
}

string MemoryDataRegister::returnInstruction() {
    return instructionForNextCC;
}