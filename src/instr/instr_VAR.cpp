#include "compiler.hpp"
#include "common.hpp"

void Compiler::instr_var () {

    /*
    0  +1 +2 +3
    var x  =  5
    var x
    */

    int defaultValue = 0;

    tPtr++;
    int addr = variableAddressLookup[Tokens[tPtr]];

    // default: set to zero
    if (Tokens[tPtr +1] == RW.RW_operator_EQ) {
        tPtr += 2;
        defaultValue = std::stoi(Tokens[tPtr]);
    }

    out += move_to(addr);
    out += CS.setToZero;

    if (defaultValue) {
        out += add_n_chars(defaultValue, BFO.plus[0]);
    }

    

}