#include "compiler.hpp"
#include "common.hpp"

void instr_cout (
    std::string &out,
    std::string &tempStr,
    std::vector<int> &tempIntVect,
    std::array<int, 2> &tempReservedArea,
    int &tPtr,
    int &ptrPosition,
    int &tempInt
    ) {

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