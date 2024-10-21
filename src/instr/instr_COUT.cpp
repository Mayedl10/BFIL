#include "compiler.hpp"
#include "common.hpp"

void Compiler::instr_cout () {

    tempStr = "";
    tempReservedArea = {};
    tempIntVect.clear();
    tempInt = 0;

    tPtr++;
    get_cur_tok();

    tempReservedArea = find_reserved(reserved, ORM.cout);

    tempStr += move_to(tempReservedArea[0]);

    // add_n_chars(amount, '+');

    for (char c: curTok) {
        tempInt = c;
        tempStr += CS.setToZero;
        tempStr += add_n_chars(tempInt, '+');
        tempStr += BFO.asciiOut;
    }

    out += tempStr+CS.setToZero;

}