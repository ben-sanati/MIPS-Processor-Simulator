#include <stdio.h>

int main() {
    int x = 15;
    int y = 10;
    int sum = x + y;
    int increment = 0;
    if(sum == 25) {
        for(int i = 0; i < 5; i++) {
            increment += sum;
        }
    }

    printf("Sum: %d, Increment: %d", sum, increment);

    return 0;
}
