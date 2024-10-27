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

void Compiler::instr_endIf () {

    // 4.
    out += move_to(0);
    out += CS.setToZero;

    // 5.
    out += BFO.closeBr;

}