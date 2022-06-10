#ifndef COMPENGCW_CONTROL_H
#define COMPENGCW_CONTROL_H

#include <iostream>
#include <string>

using namespace std;

class Control {
private:
    //control signals, each will need there own method to return current value
    int PCWriteCond;
    int PCWrite;
    int IorD;
    bool MemRead;
    bool MemWrite;
    int MemtoReg;
    bool IRWrite;
    int PCSource;
    int ALUOp;
    int ALUSrcA;
    int ALUSrcB;
    bool RegWrite;
    int RegDst;
public:
    Control();
    //cycle 1 and 2 are the same for everything
    //yet to know what the instruction is in cc1 and 2
    void instructionFetch();
    void instructionDecode();
    string instructionDecodeOperation(string, string);

    //we now know the operation being undertaken and therefore must run cc3, 4 and 5 (if required)

    //beq
    void beqExecute();
    //add
    void addExecute();
    void addWriteBack();
    //jr
    void jrExecute();
    void jrWriteBack();
    //j and jal
    void jJalExecute();
    //sw, lw and addi
    void swLwAddiExecute();                             //same for both sw and lw but cc4 (and 5 for lw) is not the same
    void swMemoryAccess();
    void lwMemoryAccess();
    void lwWriteBack();
    //addi
    void addiWBWriteBack();

    //get signal values
    int returnPCWriteCondition();
    int returnPCWrite();
    int returnIorD();
    bool returnMemRead();
    bool returnMemWrite();
    int returnMemtoReg();
    bool returnIRWrite();
    int returnPCSource();
    int returnALUOp();
    int returnALUSrcA();
    int returnALUSrcB();
    bool returnRegWrite();
    int returnRegDst();
};


#endif //COMPENGCW_CONTROL_H
