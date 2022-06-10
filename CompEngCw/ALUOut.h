//
// Created by benja on 09/04/2021.
//

#ifndef COMPENGCW_ALUOUT_H
#define COMPENGCW_ALUOUT_H

#include <iostream>
#include <string>

using namespace std;

class ALUOut {
private:
    int ALUresultNextCC;
public:
    ALUOut();
    void inputALUResult(int);
    //produce return method
    int returnALUResult();
};


#endif //COMPENGCW_ALUOUT_H
