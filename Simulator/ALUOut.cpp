//
// Created by benja on 09/04/2021.
//

#include "ALUOut.h"

ALUOut::ALUOut() {
    ALUresultNextCC = 0;
}

void ALUOut::inputALUResult(int ALUresult) {
    ALUresultNextCC = ALUresult;
    //cout << "ALUOut Value: " << ALUresultNextCC << endl;
}

int ALUOut::returnALUResult() {
    return ALUresultNextCC;
}