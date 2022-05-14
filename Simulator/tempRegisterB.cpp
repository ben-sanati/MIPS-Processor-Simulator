//
// Created by benja on 09/04/2021.
//

#include "tempRegisterB.h"

void tempRegisterB::inputRtValue(int rs) {
    nextRtValue = rs;
}

int tempRegisterB::returnBValue() {
    return nextRtValue;
}