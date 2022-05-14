//
// Created by benja on 09/04/2021.
//

#ifndef COMPENGCW_MEMORYDATAREGISTER_H
#define COMPENGCW_MEMORYDATAREGISTER_H

#include <iostream>
#include <string>

using namespace std;

class MemoryDataRegister {
private:
    string instructionForNextCC;
public:
    void inputInstruction(string);
    //produce return method
    string returnInstruction();
};


#endif //COMPENGCW_MEMORYDATAREGISTER_H
