//
// Created by benja on 08/04/2021.
//

#ifndef COMPENGCW_ALU_H
#define COMPENGCW_ALU_H

#include <iostream>
#include <string>

using namespace std;

class ALU {
private:
    bool ALUzero;
    int ALUresult, ALUinputA, ALUinputB;
    basic_string<char> stringInputB;
    bool useStringB;
public:
    ALU();
    //find inputs and calculate outputs
    void aluInputs(int, int, int, int, int, string);
    void aluResultVal(int, bool);

    //return methods, must return ALUzero and ALUresult
    bool returnALUzero();
    int returnALUResult();
};


#endif //COMPENGCW_ALU_H
