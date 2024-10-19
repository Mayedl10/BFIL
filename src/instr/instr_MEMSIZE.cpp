#include "compiler.hpp"
#include "common.hpp"
#include "instr.hpp"

void instr_memsize (
    std::string &out,
    std::string &tempStr,
    std::vector<int> &tempIntVect,
    std::array<int, 2> &tempReservedArea,
    int &tPtr,
    int &ptrPosition,
    int &memsize
    ) {

    tPtr++;
    get_cur_tok();
    memsize = hex_to_int(curTok);
}