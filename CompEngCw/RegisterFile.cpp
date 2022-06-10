//
// Created by benja on 09/04/2021.
//

#include "RegisterFile.h"

RegisterFile::RegisterFile(int finalAddress) {
    //initialise all registers
    reg = 0;
    regWriteNumber = 0;
    regReadNumber1 = 0;
    regReadNumber2 = 0;
    writeData = 0;
}

void RegisterFile::regOperations(string rs, string rt, string imm, string MDROut, int ALUOut, int RegDst, int MemtoReg, bool RegWrite) {
    regReadNumber1 = stoi(rs, 0, 2);
    regReadNumber2 = stoi(rt, 0, 2);
    //implement WR multiplexer
    switch(RegDst) {
        case 0 :
            regWriteNumber = stoi(rt, 0, 2);
            break;
        case 1 :
            //first, get rd from imm
            string rd = imm.substr(0, 5);
            regWriteNumber = stoi(rd, 0, 2);
            break;
    }
    //implement WD multiplexer
    switch(MemtoReg) {
        case 0 :
            writeData = ALUOut;
            break;
        case 1 :
            writeData = stoi(MDROut, 0, 2);
            break;
    }

    if(RegWrite) {
        reg = writeData;
        //cout << "Writing to: " << writeReg << " Data: " << writeData << endl;
    }
}

int RegisterFile::returnReg() {
    return reg;                                             //register value
}

int RegisterFile::returnRegWriteNumber() {
    return regWriteNumber;                                  //register number
}

int RegisterFile::returnReadNumber1() {
    return regReadNumber1;                                  //register number
}

int RegisterFile::returnReadNumber2() {
    return regReadNumber2;                                  //register number
}