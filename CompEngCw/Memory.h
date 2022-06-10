//
// Created by benja on 08/04/2021.
//

#ifndef COMPENGCW_MEMORY_H
#define COMPENGCW_MEMORY_H

#include <iostream>
#include <string>

using namespace std;

class Memory {
private:
    string instructions[1000];
    string memory[1000];
    bool addressLabel[1000];
    int address;
    int finalAddress;
public:
    Memory();         //sets up all instructions in memory

    void instructionstoStack(string, int);
    string returnAddressInstruction(int, int, int, int, bool, bool);
    int updateFinalAddress();
    bool* returnAddressLabel();
    string returnMemData();
};


#endif //COMPENGCW_MEMORY_H
