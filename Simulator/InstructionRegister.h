//
// Created by benja on 08/04/2021.
//

#ifndef COMPENGCW_INSTRUCTIONREGISTER_H
#define COMPENGCW_INSTRUCTIONREGISTER_H

#include <iostream>
#include <string>

using namespace std;

class InstructionRegister {
private:
    //potential data to be stored
    string opcode, rs, rt, imm;
public:
    InstructionRegister();
    void updateRegister(string);

    //return methods
    string returnOpCode();
    string returnRs();
    string returnRt();
    string returnImm();
};


#endif //COMPENGCW_INSTRUCTIONREGISTER_H
