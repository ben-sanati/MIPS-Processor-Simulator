#include <stdio.h>

int main() {
    int squareResult[100];
    int n = 1;
    squareResult[0] = 0;
    printf("Number: 0, Square: 0\n");   //initially print out 0
    for(int i = 0; i < 99; i++)
    {
        squareResult[i + 1] = squareResult[i] + n;
        n += 2;
        printf("Number: %d, Square: %d\n", i+1, squareResult[i + 1]);
    }
    return 0;
}
