//
// Created by benja on 11/04/2021.
//

#include "TestBench.h"

TestBench::TestBench() {
    expectedClkCyc = 0;
}

int TestBench::ccErrorVal(string operation, int numClkCycles) {
    int returnVal = 0;
    if((operation == "beq") || (operation == "j") || (operation == "jal")) {
        expectedClkCyc = 3;
        (expectedClkCyc == numClkCycles) ? returnVal = 0 : returnVal = 1;
    } else if((operation == "add") || (operation == "jr") || (operation == "sw") || (operation == "addi")) {
        expectedClkCyc = 4;
        (expectedClkCyc == numClkCycles) ? returnVal = 0 : returnVal = 1;
    } else if(operation == "lw") {
        expectedClkCyc = 5;
        (expectedClkCyc == numClkCycles) ? returnVal = 0 : returnVal = 1;
    }
    return returnVal;
}