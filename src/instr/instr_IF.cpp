#include "compiler.hpp"
#include "common.hpp"

/*

    -- if
    0. get source
    1. copy target into ?0
    2. moveto ?0 && [
        3. code

    -- endif
        4. goto ?0 && [-]
    5. ]


*/

#define o(x) std::cout << x << std::endl

void Compiler::instr_if () {

    // 0.
    tPtr++;
    get_cur_tok();
    tempInt = address_string_to_int(curTok);

    // 1.
    this->copy_values(tempInt, 0, true);

    // 2.
    out += move_to(0);
    out += BFO.openBr;

    // 3.
    // just let the compiler continue running

}
