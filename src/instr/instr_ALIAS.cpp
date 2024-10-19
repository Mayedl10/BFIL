#include "compiler.hpp"
#include "common.hpp"

void instr_alias (
    std::string &out,
    std::string &tempStr,
    std::vector<int> &tempIntVect,
    std::array<int, 2> &tempReservedArea,
    int &tPtr,
    int &ptrPosition
    ) {

    tPtr++;
    get_cur_tok();
    tempStr = curTok;

    tPtr+=2;
    get_cur_tok();

    for (int i = 0; i < Tokens.size(); i++) {
        if (Tokens[i] == curTok) {
            Tokens[i] = tempStr;
        }
    }

}