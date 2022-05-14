//
// Created by benja on 11/04/2021.
//

#ifndef COMPENGCW_TESTBENCH_H
#define COMPENGCW_TESTBENCH_H

#include <iostream>
#include <string>

using namespace std;

class TestBench {
private:
    int expectedClkCyc;
public:
    TestBench();
    int ccErrorVal(string, int);
};


#endif //COMPENGCW_TESTBENCH_H
