#include "compiler.hpp"
#include "common.hpp"

void Compiler::instr_aout () {

    tPtr++;
    get_cur_tok();
    tempInt = std::stoi(curTok);    // amount of parameters
    tempIntVect.clear();
    tempStr = "";

    for (int i = 0; i < tempInt; i++) {
        tPtr++;
        get_cur_tok();
        tempIntVect.push_back(address_string_to_int(curTok));
    }

    // brainfuck code generation
    // how?: iterate through tempIntVect, move to ?i:tempIntVect and use BFO.asciiOut

    for (int i = 0; i < tempIntVect.size(); i++) {
        tempStr += move_to(tempIntVect[i]);
        tempStr += BFO.asciiOut;
    }

    out += tempStr;

}