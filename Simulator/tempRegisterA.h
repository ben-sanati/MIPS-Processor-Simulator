//
// Created by benja on 09/04/2021.
//

#ifndef COMPENGCW_TEMPREGISTERA_H
#define COMPENGCW_TEMPREGISTERA_H

#include <iostream>
#include <string>

using namespace std;

class tempRegisterA {
private:
    int nextRsValue;
public:
    void inputRsValue(int);
    //produce return method
    int returnAValue();
};


#endif //COMPENGCW_TEMPREGISTERA_H
