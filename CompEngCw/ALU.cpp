//
// Created by benja on 08/04/2021.
//

#include "ALU.h"
ALU::ALU() {
    ALUzero = false;
    ALUresult = 0;
    ALUinputA = 0;
    ALUinputB = 0;
    useStringB = false;
}

void ALU::aluInputs(int currentAddress, int regA, int regB, int ALUSrcA, int ALUSrcB, string imm) {
    //get input A from multiplexer
    switch(ALUSrcA) {
        case 0 :
            ALUinputA = currentAddress;
            break;
        case 1 :
            ALUinputA = regA;
            break;
    }
    //get input B from multiplexer
    switch(ALUSrcB) {
        case 0 :
            ALUinputB = regB;
            break;
        case 1 :
            ALUinputB = 4;
            break;
        case 2 :
            stringInputB = imm;
            for(int i = 0; i < 16; i++) {                                           //make 32 bits long (sign-extend)
                stringInputB = imm.insert(0, "0");
            }
            useStringB = true;                                                        //identifier for string use
            break;
        case 3:
            stringInputB = imm;
            for(int i = 0; i < 14; i++) {                                           //make 30 bits long (sign-extend)
                stringInputB = imm.insert(0, "0");
            }
            for(int i = 0; i < 2; i++) {
                stringInputB.push_back('0');                                    //shift left 2
            }
            useStringB = true;                                                        //identifier for string use
            break;
    }
    //cout << "A: " << ALUinputA << " B: " << ALUinputB << endl;
}

void ALU::aluResultVal(int ALUOp, bool immSign) {
    int tempB;
    //implement ALUControl
    switch(ALUOp) {
        case 0 :                                                                    //add
            if(useStringB) {
                //convert binary to decimal
                tempB = stoi(stringInputB, 0, 2);
                (immSign) ? tempB = -tempB : tempB;

                ALUresult = ALUinputA + tempB;
            } else {
                ALUresult = ALUinputA + ALUinputB;
            }
            break;
        case 1 :                                                                    //subtract
            if(useStringB) {
                //convert binary to decimal
                tempB = stoi(stringInputB, 0, 2);
                (immSign) ? tempB = -tempB : tempB;

                ALUresult = ALUinputA - tempB;
                (ALUresult == 0) ? ALUzero = true : ALUzero = false;
            } else {
                ALUresult = ALUinputA - ALUinputB;
                (ALUresult == 0) ? ALUzero = true : ALUzero = false;
            }
            break;
        case 2 :                                                                    //use function
            //in this case both add and jr add the value in reg A to the value in reg B, therefore this is the same as case 0
            if(useStringB) {
                //convert binary to decimal
                tempB = stoi(stringInputB, 0, 2);
                (immSign) ? tempB = -tempB : tempB;

                ALUresult = ALUinputA + tempB;
            } else {
                ALUresult = ALUinputA + ALUinputB;
            }
            break;
        default:
            if(useStringB) {
                //convert binary to decimal
                tempB = stoi(stringInputB, 0, 2);
                (immSign) ? tempB = -tempB : tempB;

                ALUresult = ALUinputA + tempB;
            } else {
                ALUresult = ALUinputA + ALUinputB;
            }
            break;
    }
    useStringB = 0;
}

bool ALU::returnALUzero() {
    return ALUzero;
}

int ALU::returnALUResult() {
    return ALUresult;
}