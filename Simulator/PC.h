//
// Created by benja on 08/04/2021.
//

#include <iostream>
#include <string>

#ifndef COMPENGCW_PC_H
#define COMPENGCW_PC_H

using namespace std;

class PC {
private:
    int currentAddress, finalAddress;                                           //potentially dont need finalAddress here-----------------------------------
public:
    PC(int);

    void updatePC(int, int, string, int, int, int, bool);

    //return methods
    int returnCurrentAddress();
};


#endif //COMPENGCW_PC_H
