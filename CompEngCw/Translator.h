#ifndef COMPENGCW_TRANSLATOR_H
#define COMPENGCW_TRANSLATOR_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class Translator {
private:
    string instruction[1000], op[1000], rd[1000], rs[1000], rt[1000], funct[1000];             //all instruction related
    string binOp[1000], binRdReg[1000], binRsReg[1000], binRtReg[1000], binFunct[1000];           //binary codes
    int decOp[1000];                                                                      //opcode decimal
    int finalAddress = -4;                                                               //final address
    int regRdNum, regRsNum, regRtNum;
    string binImm[1000];
    string beqImm;
    int immDec;
    string shamt;
    string binInstruction[1000];                                                          //binary instruction
    bool immSign[1000];
public:
    Translator();
    void getFileInput();
    void opEncoder(int);
    void regRdEncoder(int);
    void regRsEncoder(int);
    void regRtEncoder(int);
    void immEncoder(int);
    void beqImmEncoder(int);

    //return functions
    string returnBinaryInstruction(int);
    int returnFinalAddress();
    int returnDecimalOpcode(int);
    bool* returnImmSign();

};


#endif //COMPENGCW_TRANSLATOR_H
