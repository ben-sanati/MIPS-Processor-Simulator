#include "Translator.h"

Translator::Translator() {
    /*
     * Different example assembly programs include:
     *      ../ShowcaseProgram.txt
     *      ../TestProgram1.txt
     *      ../TestProgram2.txt
     *      ../TestProgram3.txt
     * For information regarding the different example programs please see the GitHub repo
     */

    fstream txtFile("../ShowcaseProgram.txt", ios::in);  //file being read

    for(int i = 0; i < 1000; i++)
        immSign[i] = false;

    if (txtFile.is_open()) {
        while(txtFile) {
            finalAddress += 4;                                                           //read file and set each line to instruction array
            getline(txtFile, instruction[finalAddress / 4]);
        }
        txtFile.close();
    }                                                                   //read each line

    for(int num = 0; num < sizeof(op)/sizeof(op[0]); num++) {                                      //read each word
        istringstream ss(instruction[num]);
        ss >> op[num];

        opEncoder(num);

        if(op[num] == "add") {
            //add %rd, $rs, $rt
            shamt = "00000";

            //rd
            ss >> rd[num];
            char c = rd[num].back();
            if(c == ',' && rd[num] != "$zero,") {
                rd[num].erase(3);
            } else if(rt[num] == "$zero,")
                rd[num].erase(5);

            regRdEncoder(num);

            //rs
            ss >> rs[num];
            char d = rs[num].back();
            //remove commas
            if(d == ',' && rs[num] != "$zero,") {
                rs[num].erase(3);
            } else if(rt[num] == "$zero,")
                rs[num].erase(5);

            regRsEncoder(num);

            //rt
            ss >> rt[num];
            char e = rt[num].back();
            if(e == ',' && rt[num] != "$zero,") {
                rt[num].erase(3);
            } else if(rt[num] == "$zero,")
                rt[num].erase(5);

            regRtEncoder(num);

            funct[num] = "100000";
            binInstruction[num] = binOp[num] + binRsReg[num] + binRtReg[num] + binRdReg[num] + shamt + funct[num];
            //cout << num * 4 << ": " << binInstruction[num] << " " << binInstruction[num].size() << endl;
        } else if (op[num] == "jr"){
            //jr $rs
            binRtReg[num] = "00000";
            binRdReg[num] = "00000";
            shamt = "00000";
            funct[num] = "001000";

            //rs
            ss >> rs[num];
            char d = rs[num].back();
            //remove commas
            if(d == ',' && rs[num] != "$zero,") {
                rs[num].erase(3);
            } else if(rt[num] == "$zero,")
                rs[num].erase(5);

            regRsEncoder(num);
            binInstruction[num] = binOp[num] + binRsReg[num] + binRtReg[num] + binRdReg[num] + shamt + funct[num];
            //cout << num * 4 << ": " << binInstruction[num] << " " << binInstruction[num].size() << endl;
        } else if (op[num] == "addi"){
            //addi $rt, $rs, imm

            //rt
            ss >> rt[num];
            char e = rt[num].back();
            if(e == ',' && rt[num] != "$zero,") {
                rt[num].erase(3);
            } else if(rt[num] == "$zero,")
                rt[num].erase(5);

            regRtEncoder(num);

            //rs
            ss >> rs[num];
            char d = rs[num].back();
            //remove commas
            if(d == ',' && rs[num] != "$zero,") {
                rs[num].erase(3);
            } else if(rs[num] == "$zero,")
                rs[num].erase(5);

            regRsEncoder(num);

            //get the immediate value
            string imm;
            ss >> imm;
            immDec = stoi(imm);

            immEncoder(num);
            binInstruction[num] = binOp[num] + binRsReg[num] + binRtReg[num] + binImm[num];
            //cout << num * 4 << ": " << binInstruction[num] << " " << binInstruction[num].size() << endl;
        } else if (op[num] == "beq"){
            //beq $rs, $rt, imm (translator only works with register values, not digits)
            //rs
            //cout << instruction[num] << endl;

            ss >> rs[num];
            char d = rs[num].back();
            //remove commas
            if(d == ',' && rs[num] != "$zero,") {
                rs[num].erase(3);
            } else if(rs[num] == "$zero,")
                rs[num].erase(5);

            regRsEncoder(num);

            //rt
            ss >> rt[num];
            char e = rt[num].back();
            if(e == ',' && rt[num] != "$zero,") {
                rt[num].erase(3);
            } else if(rt[num] == "$zero,")
                rt[num].erase(5);

            regRtEncoder(num);

            //imm
            ss >> beqImm;
            beqImmEncoder(num);
            binInstruction[num] = binOp[num] + binRsReg[num] + binRtReg[num] + binImm[num];
            //cout << num * 4 << ": " << binInstruction[num] << " " << binInstruction[num].size() << endl;
        } else if (op[num] == "sw"){
            //sw $rt, imm($rs)
            //rt
            ss >> rt[num];
            char e = rt[num].back();
            if(e == ',' && rt[num] != "$zero,") {
                rt[num].erase(3);
            } else if(rt[num] == "$zero,")
                rt[num].erase(5);

            regRtEncoder(num);

            //find '('
            //set everything up to '(' to immDec (not including '(')
            //set $rs to thing between brackets

            string wordInfo[100];
            ss >> wordInfo[num];
            //imm
            char immediate;
            int positionofBrack = wordInfo[num].find("(");
            immediate = wordInfo[num].at(positionofBrack - 1);
            immDec = immediate - 48;                                            //conversion from ASCII codes

            immEncoder(num);

            //rs
            string rsTemp;
            rsTemp = wordInfo[num].substr(positionofBrack);
            rsTemp = rsTemp.substr(1);
            rs[num] = rsTemp.substr(0, rsTemp.size() - 1);

            regRsEncoder(num);

            binInstruction[num] = binOp[num] + binRsReg[num] + binRtReg[num] + binImm[num];
            //cout << num * 4 << ": " << binInstruction[num] << " " << binInstruction[num].size() << endl;
        } else if (op[num] == "lw"){
            //lw $rt, imm($rs)
            //rt
            ss >> rt[num];
            char e = rt[num].back();
            if(e == ',' && rt[num] != "$zero,") {
                rt[num].erase(3);
            } else if(rt[num] == "$zero,")
                rt[num].erase(5);

            regRtEncoder(num);

            //find '('
            //set everything up to '(' to immDec (not including '(')
            //set $rs to thing between brackets

            string wordInfo[100];
            ss >> wordInfo[num];
            //imm
            char immediate;
            int positionofBrack = wordInfo[num].find("(");
            immediate = wordInfo[num].at(positionofBrack - 1);
            immDec = immediate - 48;                                            //conversion from ASCII codes

            immEncoder(num);

            //rs
            string rsTemp;
            rsTemp = wordInfo[num].substr(positionofBrack);
            rsTemp = rsTemp.substr(1);
            rs[num] = rsTemp.substr(0, rsTemp.size() - 1);

            regRsEncoder(num);

            binInstruction[num] = binOp[num] + binRsReg[num] + binRtReg[num] + binImm[num];
            //cout << num * 4 << ": " << binInstruction[num] << " " << binInstruction[num].size() << endl;
        } else if (op[num] == "j"){
            //j addr
            ss >> beqImm;
            beqImmEncoder(num);
            binInstruction[num] = binOp[num] + binImm[num];
            //cout << num * 4 << ": " << binInstruction[num] << " " << binInstruction[num].size() << endl;
        } else if (op[num] == "jal"){
            //jal addr
            ss >> beqImm;
            beqImmEncoder(num);
            binInstruction[num] = binOp[num] + binImm[num];
            //cout << num * 4 << ": " << binInstruction[num] << " " << binInstruction[num].size() << endl;
        }
    }
}

void Translator::getFileInput() {
    for(int i = 0; i < finalAddress / 4; i++)
        cout << i * 4 << ": " << instruction[i] << endl;
}

void Translator::opEncoder(int i) {
    if(op[i] == "add") {
        binOp[i] = "000000";
        decOp[i] = 0;
        binFunct[i] = "100000";
    } else if (op[i] == "jr"){
        binOp[i] = "000000";
        decOp[i] = 0;
        binFunct[i] = "001000";
    } else if (op[i] == "addi"){
        binOp[i] = "001000";
        decOp[i] = 8;
    } else if (op[i] == "beq"){
        binOp[i] = "000100";
        decOp[i] = 4;
    } else if (op[i] == "sw"){
        binOp[i] = "101011";
        decOp[i] = 43;
    } else if (op[i] == "lw"){
        binOp[i] = "010111";
        decOp[i] = 23;
    } else if (op[i] == "j"){
        binOp[i] = "000010";
        decOp[i] = 2;
    } else if (op[i] == "jal"){
        binOp[i] = "000011";
        decOp[i] = 3;
    }
}

void Translator::regRdEncoder(int num) {
    if(rd[num] == "$zero") {
        regRdNum = 0;
        binRdReg[num] = "00000";
    } else if(rd[num] == "$at") {
        regRdNum = 1;
        binRdReg[num] = "00001";
    } else if(rd[num] == "$v0") {
        regRdNum = 2;
        binRdReg[num] = "00010";
    } else if(rd[num] == "$v1") {
        regRdNum = 3;
        binRdReg[num] = "00011";
    } else if(rd[num] == "$a0") {
        regRdNum = 4;
        binRdReg[num] = "00100";
    } else if(rd[num] == "$a1") {
        regRdNum = 5;
        binRdReg[num] = "00101";
    } else if(rd[num] == "$a2") {
        regRdNum = 6;
        binRdReg[num] = "00110";
    } else if(rd[num] == "$a3") {
        regRdNum = 7;
        binRdReg[num] = "00111";
    } else if(rd[num] == "$t0") {
        regRdNum = 8;
        binRdReg[num] = "01000";
    } else if(rd[num] == "$t1") {
        regRdNum = 9;
        binRdReg[num] = "01001";
    } else if(rd[num] == "$t2") {
        regRdNum = 10;
        binRdReg[num] = "01010";
    } else if(rd[num] == "$t3") {
        regRdNum = 11;
        binRdReg[num] = "01011";
    } else if(rd[num] == "$t4") {
        regRdNum = 12;
        binRdReg[num] = "01100";
    } else if(rd[num] == "$t5") {
        regRdNum = 13;
        binRdReg[num] = "01101";
    } else if(rd[num] == "$t6") {
        regRdNum = 14;
        binRdReg[num] = "01110";
    } else if(rd[num] == "$t7") {
        regRdNum = 15;
        binRdReg[num] = "01111";
    } else if(rd[num] == "$s0") {
        regRdNum = 16;
        binRdReg[num] = "10000";
    } else if(rd[num] == "$s1") {
        regRdNum = 17;
        binRdReg[num] = "10001";
    } else if(rd[num] == "$s2") {
        regRdNum = 18;
        binRdReg[num] = "10010";
    } else if(rd[num] == "$s3") {
        regRdNum = 19;
        binRdReg[num] = "10011";
    } else if(rd[num] == "$s4") {
        regRdNum = 20;
        binRdReg[num] = "10100";
    } else if(rd[num] == "$s5") {
        regRdNum = 21;
        binRdReg[num]= "10101";
    } else if(rd[num] == "$s6") {
        regRdNum = 22;
        binRdReg[num] = "10110";
    } else if(rd[num] == "$s7") {
        regRdNum = 23;
        binRdReg[num] = "10111";
    } else if(rd[num] == "$t8") {
        regRdNum = 24;
        binRdReg[num] = "11000";
    } else if(rd[num] == "$t9") {
        regRdNum = 25;
        binRdReg[num] = "11001";
    } else if(rd[num] == "$k0") {
        regRdNum = 26;
        binRdReg[num] = "11010";
    } else if(rd[num] == "$k1") {
        regRdNum = 27;
        binRdReg[num] = "11011";
    } else if(rd[num] == "$gp") {
        regRdNum = 28;
        binRdReg[num] = "11100";
    } else if(rd[num] == "$sp") {
        regRdNum = 29;
        binRdReg[num] = "11101";
    } else if(rd[num] == "$fp") {
        regRdNum = 30;
        binRdReg[num] = "11110";
    } else if(rd[num] == "$ra") {
        regRdNum = 31;
        binRdReg[num] = "11111";
    }
}

void Translator::regRsEncoder(int num) {
    if(rs[num] == "$zero") {
        regRsNum = 0;
        binRsReg[num] = "00000";
    } else if(rs[num] == "$at") {
        regRsNum = 1;
        binRsReg[num] = "00001";
    } else if(rs[num] == "$v0") {
        regRsNum = 2;
        binRsReg[num] = "00010";
    } else if(rs[num] == "$v1") {
        regRsNum = 3;
        binRsReg[num] = "00011";
    } else if(rs[num] == "$a0") {
        regRsNum = 4;
        binRsReg[num] = "00100";
    } else if(rs[num] == "$a1") {
        regRsNum = 5;
        binRsReg[num] = "00101";
    } else if(rs[num] == "$a2") {
        regRsNum = 6;
        binRsReg[num] = "00110";
    } else if(rs[num] == "$a3") {
        regRsNum = 7;
        binRsReg[num] = "00111";
    } else if(rs[num] == "$t0") {
        regRsNum = 8;
        binRsReg[num] = "01000";
    } else if(rs[num] == "$t1") {
        regRsNum = 9;
        binRsReg[num] = "01001";
    } else if(rs[num] == "$t2") {
        regRsNum = 10;
        binRsReg[num] = "01010";
    } else if(rs[num] == "$t3") {
        regRsNum = 11;
        binRsReg[num] = "01011";
    } else if(rs[num] == "$t4") {
        regRsNum = 12;
        binRsReg[num] = "01100";
    } else if(rs[num] == "$t5") {
        regRsNum = 13;
        binRsReg[num] = "01101";
    } else if(rs[num] == "$t6") {
        regRsNum = 14;
        binRsReg[num] = "01110";
    } else if(rs[num] == "$t7") {
        regRsNum = 15;
        binRsReg[num] = "01111";
    } else if(rs[num] == "$s0") {
        regRsNum = 16;
        binRsReg[num] = "10000";
    } else if(rs[num] == "$s1") {
        regRsNum = 17;
        binRsReg[num] = "10001";
    } else if(rs[num] == "$s2") {
        regRsNum = 18;
        binRsReg[num] = "10010";
    } else if(rs[num] == "$s3") {
        regRsNum = 19;
        binRsReg[num] = "10011";
    } else if(rs[num] == "$s4") {
        regRsNum = 20;
        binRsReg[num] = "10100";
    } else if(rs[num] == "$s5") {
        regRsNum = 21;
        binRsReg[num]= "10101";
    } else if(rs[num] == "$s6") {
        regRsNum = 22;
        binRsReg[num] = "10110";
    } else if(rs[num] == "$s7") {
        regRsNum = 23;
        binRsReg[num] = "10111";
    } else if(rs[num] == "$t8") {
        regRsNum = 24;
        binRsReg[num] = "11000";
    } else if(rs[num] == "$t9") {
        regRsNum = 25;
        binRsReg[num] = "11001";
    } else if(rs[num] == "$k0") {
        regRsNum = 26;
        binRsReg[num] = "11010";
    } else if(rs[num] == "$k1") {
        regRsNum = 27;
        binRsReg[num] = "11011";
    } else if(rs[num] == "$gp") {
        regRsNum = 28;
        binRsReg[num] = "11100";
    } else if(rs[num] == "$sp") {
        regRsNum = 29;
        binRsReg[num] = "11101";
    } else if(rs[num] == "$fp") {
        regRsNum = 30;
        binRsReg[num] = "11110";
    } else if(rs[num] == "$ra") {
        regRsNum = 31;
        binRsReg[num] = "11111";
    }
}

void Translator::regRtEncoder(int num) {
    if(rt[num] == "$zero") {
        regRtNum = 0;
        binRtReg[num] = "00000";
    } else if(rt[num] == "$at") {
        regRtNum = 1;
        binRtReg[num] = "00001";
    } else if(rt[num] == "$v0") {
        regRtNum = 2;
        binRtReg[num] = "00010";
    } else if(rt[num] == "$v1") {
        regRtNum = 3;
        binRtReg[num] = "00011";
    } else if(rt[num] == "$a0") {
        regRtNum = 4;
        binRtReg[num] = "00100";
    } else if(rt[num] == "$a1") {
        regRtNum = 5;
        binRtReg[num] = "00101";
    } else if(rt[num] == "$a2") {
        regRtNum = 6;
        binRtReg[num] = "00110";
    } else if(rt[num] == "$a3") {
        regRtNum = 7;
        binRtReg[num] = "00111";
    } else if(rt[num] == "$t0") {
        regRtNum = 8;
        binRtReg[num] = "01000";
    } else if(rt[num] == "$t1") {
        regRtNum = 9;
        binRtReg[num] = "01001";
    } else if(rt[num] == "$t2") {
        regRtNum = 10;
        binRtReg[num] = "01010";
    } else if(rt[num] == "$t3") {
        regRtNum = 11;
        binRtReg[num] = "01011";
    } else if(rt[num] == "$t4") {
        regRtNum = 12;
        binRtReg[num] = "01100";
    } else if(rt[num] == "$t5") {
        regRtNum = 13;
        binRtReg[num] = "01101";
    } else if(rt[num] == "$t6") {
        regRtNum = 14;
        binRtReg[num] = "01110";
    } else if(rt[num] == "$t7") {
        regRtNum = 15;
        binRtReg[num] = "01111";
    } else if(rt[num] == "$s0") {
        regRtNum = 16;
        binRtReg[num] = "10000";
    } else if(rt[num] == "$s1") {
        regRtNum = 17;
        binRtReg[num] = "10001";
    } else if(rt[num] == "$s2") {
        regRtNum = 18;
        binRtReg[num] = "10010";
    } else if(rt[num] == "$s3") {
        regRtNum = 19;
        binRtReg[num] = "10011";
    } else if(rt[num] == "$s4") {
        regRtNum = 20;
        binRtReg[num] = "10100";
    } else if(rt[num] == "$s5") {
        regRtNum = 21;
        binRtReg[num]= "10101";
    } else if(rt[num] == "$s6") {
        regRtNum = 22;
        binRtReg[num] = "10110";
    } else if(rt[num] == "$s7") {
        regRtNum = 23;
        binRtReg[num] = "10111";
    } else if(rt[num] == "$t8") {
        regRtNum = 24;
        binRtReg[num] = "11000";
    } else if(rt[num] == "$t9") {
        regRtNum = 25;
        binRtReg[num] = "11001";
    } else if(rt[num] == "$k0") {
        regRtNum = 26;
        binRtReg[num] = "11010";
    } else if(rt[num] == "$k1") {
        regRtNum = 27;
        binRtReg[num] = "11011";
    } else if(rt[num] == "$gp") {
        regRtNum = 28;
        binRtReg[num] = "11100";
    } else if(rt[num] == "$sp") {
        regRtNum = 29;
        binRtReg[num] = "11101";
    } else if(rt[num] == "$fp") {
        regRtNum = 30;
        binRtReg[num] = "11110";
    } else if(rt[num] == "$ra") {
        regRtNum = 31;
        binRtReg[num] = "11111";
    }
}

void Translator::immEncoder(int num) {
    //convert decimal to binary
    if(immDec < 0) {
        immSign[num] = true;
    } else
        immSign[num] = false;

    if(immDec == 0) {
        binImm[num] = "0";
    }
    while(immDec!=0) {                                                          //this decimal to binary converter was found on stack overflow: https://stackoverflow.com/questions/22746429/c-decimal-to-binary-converting
        binImm[num] = (immDec % 2 == 0 ? "0" : "1") + binImm[num];
        immDec /= 2;
    }

    for(int bitLength = binImm[num].size(); bitLength < 16; bitLength++) {      //make 16 bits long
        binImm[num].insert(0, "0");
    }
}

void Translator::beqImmEncoder(int num) {
    int temp;
    for(int i = 0; i < finalAddress / 4; i++) {
        if(instruction[i].find(beqImm + ":") != string::npos) {         //look for label and return immediate (address)
            if((i * 4) == 0) {
                binImm[num] = "0";
            }

            if(op[num] == "beq") {
                temp = i - num;                                             //relative addressing
            } else {
                temp = i;                                                   //direct addressing
            }

            //cout << "Temp: " << temp << endl;
            if(temp < 0) {
                immSign[num] = true;
            } else {
                immSign[num] = false;
            }

            while(temp != 0) {                                                          //this decimal to binary converter was found on stack overflow: https://stackoverflow.com/questions/22746429/c-decimal-to-binary-converting
                binImm[num] = (temp % 2 == 0 ? "0" : "1") + binImm[num];
                temp /= 2;
            }

            if((op[num] == "beq") || (op[num] == "sw")) {
                for(int bitLength = binImm[num].size(); bitLength < 16; bitLength++) {      //make 16 bits long
                    binImm[num].insert(0, "0");
                }
            } else if ((op[num] == "j") || (op[num] == "jal")) {
                for(int bitLength = binImm[num].size(); bitLength < 26; bitLength++) {      //make 26 bits long
                    binImm[num].insert(0, "0");
                }
            }
        }
    }
}

string Translator::returnBinaryInstruction(int i) {
    return binInstruction[i];
}

int Translator::returnFinalAddress() {
    return finalAddress;
}

int Translator::returnDecimalOpcode(int i) {
    return decOp[i];
}

bool* Translator::returnImmSign() {
    return immSign;
}