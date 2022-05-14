cmake_minimum_required(VERSION 3.15)
project(CompEngCw)

set(CMAKE_CXX_STANDARD 14)

add_executable(CompEngCw main.cpp Translator.cpp Translator.h Control.cpp Control.h Memory.cpp Memory.h PC.cpp PC.h InstructionRegister.cpp InstructionRegister.h ALU.cpp ALU.h MemoryDataRegister.cpp MemoryDataRegister.h ALUOut.cpp ALUOut.h RegisterFile.cpp RegisterFile.h tempRegisterA.cpp tempRegisterA.h tempRegisterB.cpp tempRegisterB.h TestBench.cpp TestBench.h)