//
// Created by benja on 08/04/2021.
//

#include "Memory.h"

Memory::Memory() {
    for(int i = 0; i < 1000; i++)
        addressLabel[i] = false;
    address = 0;
    finalAddress = 0;
}

void Memory::instructionstoStack(string input, int addressInit) {
    //cout << addressInit << ": " << input << endl;
    finalAddress = addressInit;
    if(input.length() == 32) {
        //cout << finalAddress * 4 << ": ";
        for(int i = (finalAddress * 4); i < (finalAddress * 4) + 4; i++) {
            instructions[i] = input.substr((i % 4) * 8, 8);
            //cout << instructions[i];
        }
        //cout << endl;
    } else {
        addressLabel[addressInit] = true;
    }
    finalAddress = (finalAddress * 4);
}

string Memory::returnAddressInstruction(int IorD, int pcAddress, int ALUOut, int regB, bool MemRead, bool MemWrite) {
    if(MemRead) {
        switch(IorD) {
            case 0 :                                        //PC address to be read
                address = pcAddress;
                return instructions[address] + instructions[address + 1] + instructions[address + 2] + instructions[address + 3];
            case 1 :                                        //ALUout address to be read and/or write
                address = ALUOut;
                return memory[address];
        }
    } else if(MemWrite) {                                                           //setup MemData
        address = ALUOut;
        while(regB!= 0) {                                                          //this decimal to binary converter was found on stack overflow: https://stackoverflow.com/questions/22746429/c-decimal-to-binary-converting
            memory[address] = (regB % 2 == 0 ? "0" : "1") + memory[address];
            regB /= 2;
        }
        for(int bitLength = memory[address].size(); bitLength < 32; bitLength++) {      //make 32 bits long
            memory[address].insert(0, "0");
        }
    }
    return "00000000000000000000000000000000";
}

int Memory::updateFinalAddress() {
    return finalAddress;
}

bool* Memory::returnAddressLabel() {                                                                //tells us if there is a label
    return addressLabel;
}

string Memory::returnMemData() {
    return memory[address];
}