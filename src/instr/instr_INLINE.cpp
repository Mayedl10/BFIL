#include "compiler.hpp"
#include "common.hpp"

void instr_inline (
    std::string &out,
    std::string &tempStr,
    std::vector<int> &tempIntVect,
    std::array<int, 2> &tempReservedArea,
    int &tPtr,
    int &ptrPosition
    ) {

    out += " ";
    tPtr++;
    get_cur_tok();

    if (string_contains(curTok, BFO.allOps)) {

        raise_compiler_warning(CompilerWarnings::reservedInlineCharacter, 
        "Your inline statement could lead to errors because it contains one of the following characters: " + BFO.allOps,
        "... 'inline " + curTok + "' ..."
        );

    }

    out += curTok;
    out += " ";

    // raise warning if the thing contains "bad" characters. (+-<>[].;)

}