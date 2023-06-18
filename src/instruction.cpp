#include <map>
#include <string>
#include <vector>
#include "raw_program.h"

bool is_imm12(int num) {
    if (num<=2047 && num>=-2048) return true;
    else return false;
}

string lw(string reg, int offset, string base_reg, string comment) {
    if (comment != "") {
        comment = "//" + comment;
    }
    if (is_imm12(offset)) {
        return "    lw " + reg + ", " + to_string(offset) + "(" + base_reg + ")\t" + comment + "\n";
    }
    else {
        string ret;
        ret += "    li s2, " + to_string(offset) + "\t" + comment + "\n";
        ret += "    add s2, " + base_reg + ", s2\t" + comment + "\n";
        ret += "    lw " + reg + ", 0(s2)\t" + comment + "\n";
        return ret;
    }
}

string sw(string reg, int offset, string base_reg, string comment) {
    if (comment != "") {
        comment = "//" + comment;
    }
    if (is_imm12(offset)) {
        return "    sw " + reg + ", " + to_string(offset) + "(" + base_reg + ")\t" + comment + "\n";
    }
    else {
        string ret;
        ret += "    li s2, " + to_string(offset) + "\t" + comment + "\n";
        ret += "    add s2, " + base_reg + ", s2\t" + comment + "\n";
        ret += "    sw " + reg + ", 0(s2)\t" + comment + "\n";
        return ret;
    }
}

string add(string des, string reg1, string reg2) {
    return "    add " + des + ", " + reg1 + ", " + reg2 + "\n";
}

string addi(string des, string reg1, int imm12) {
    if (is_imm12(imm12)) {
        return "    addi " + des + ", " + reg1 + ", " + to_string(imm12) + "\n";
    }
    else {
        string ret;
        ret += "    li s2, " + to_string(imm12) + "\n";
        ret += "    add " + des + ", " + reg1 + ", s2\n";
        return ret;
    }
}

string li(string des, int imm) {
    return "    li " + des + ", " + to_string(imm) + "\n";
}

string la(string des, string label) {
    return "    la " + des + ", " + label + "\n";
}

string mul(string rd, string rs1, string rs2) {
    return "    mul " + rd + ", " + rs1 + ", " + rs2 + "\n";
}