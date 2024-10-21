#include "compiler.hpp"
#include "common.hpp"

void Compiler::instr_memsize () {

    tPtr++;
    get_cur_tok();
    memsize = hex_to_int(curTok);
}