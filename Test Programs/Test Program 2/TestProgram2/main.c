#include <stdio.h>

int procedureFunc(int num1, int num2) {
    return num1 + num2;
}

int main() {
    int a, b, h;
    a=15;
    if(a == 5) {
        b = 12;
        h = procedureFunc(a, b);
    }
    else {
        b=-12;
        h = procedureFunc(a, b);
    }
    printf("The output is: %d", h);
    return 0;
}
