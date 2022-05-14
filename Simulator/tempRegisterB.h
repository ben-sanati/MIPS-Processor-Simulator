//
// Created by benja on 09/04/2021.
//

#ifndef COMPENGCW_TEMPREGISTERB_H
#define COMPENGCW_TEMPREGISTERB_H

#include <iostream>
#include <string>

using namespace std;

class tempRegisterB {
private:
    int nextRtValue;
public:
    void inputRtValue(int);
    //produce return method
    int returnBValue();
};


#endif //COMPENGCW_TEMPREGISTERB_H
