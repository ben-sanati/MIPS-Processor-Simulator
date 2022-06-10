//
// Created by benja on 09/04/2021.
//

#include "tempRegisterA.h"

void tempRegisterA::inputRsValue(int rs) {
    nextRsValue = rs;
}

int tempRegisterA::returnAValue() {
    return nextRsValue;
}