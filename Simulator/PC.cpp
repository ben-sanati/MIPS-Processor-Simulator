//
// Created by benja on 08/04/2021.
//

#include "PC.h"

PC::PC(int final) {
    currentAddress = 0;
    finalAddress = final;
}

void PC::updatePC(int ALUResult, int ALUOut, string imm, int PCSource, int PCWrite, int PCWriteCond, bool ALUzero) {
    if((PCWrite == 1) || ((PCWriteCond == 1) && (ALUzero == 1))) {                   //then write to PC
        switch(PCSource) {                                                           //implement multiplexer
            case 0 :
                currentAddress = ALUResult;
                break;
            case 1 :
                currentAddress = ALUOut;
                break;
            case 2 :
                currentAddress = stoi(imm, 0, 2) * 4;
                break;
        }
    }
}

int PC::returnCurrentAddress() {
    return currentAddress;
}