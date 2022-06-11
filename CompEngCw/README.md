# Simulator Code

Each module of the simulator is implemented in a class. The modules implemented are as follows:

- ALU
- ALU Out
- Control
- Instruction Register
- Memory File
- Memory Data Register
- PC
- Register File
- Registers A and B

The `runInstructions` function in [main.cpp](./main.cpp) initializes all of the modules and calls the required functions depending on the input instruction. <br />

The **Translator.cpp** class translates the input assembly code into machine code for the simulator. The assembly code is inputted as a `.txt` file. Test programs were made to ensure every instruction in the implemented IS operated as expected. The following test programs were implemented:

- **TestProgram1.txt**
    - This program produces 2 variables (`x` and `y`) and then sums them. If the sum == 25 (which it is), the program will then increment the sum 5 times in a for loop and store the result to $17 (125)
    - The output to the program should be in $s1 ($17) and it should be 125
- **TestProgram2.txt**
    - This sets a variable a ($a0) to 15, and a variable b to −12
    - It then runs a leaf procedure which increments the sum of the 2 variables
    - The expected result is in $s0 ($16) and should be 3
- **TestProgram3.txt**
    - This is a very simple program and checks that addi and add can use negative values
    - The expected result in $s0 ($16) is −2

The showcase program was the program that the simulator was marked on. The showcase program gets the squares of all integers from 0 to 99. The final square result (99) is stored in $t2
($10 → 9801) and the loaded square of 25 is stored is $t5 ($13) and is 625. This was used in conjunction with modes 0 and 2 for debugging.