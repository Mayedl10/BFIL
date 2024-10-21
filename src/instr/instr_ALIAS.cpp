#include "compiler.hpp"
#include "common.hpp"

void Compiler::instr_alias () {

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