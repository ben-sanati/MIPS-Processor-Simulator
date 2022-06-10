//
// Created by benja on 09/04/2021.
//

#ifndef COMPENGCW_REGISTERFILE_H
#define COMPENGCW_REGISTERFILE_H

#include <iostream>
#include <string>
#include <math.h>

using namespace std;

class RegisterFile {
private:
    int writeData;
    int reg, regWriteNumber, regReadNumber1, regReadNumber2;
public:
    RegisterFile(int);
    void regOperations(string, string, string, string, int, int, int, bool);

    //return methods
    int returnReg();
    int returnRegWriteNumber();
    int returnReadNumber1();
    int returnReadNumber2();
};

#endif //COMPENGCW_REGISTERFILE_H
