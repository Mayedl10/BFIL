#include "compiler.hpp"
#include "common.hpp"

void Compiler::instr_reserve () {

    tPtr++;
    get_cur_tok();
    tempReservedArea[0] = address_string_to_int(curTok);

    tPtr+=2;
    get_cur_tok();
    tempReservedArea[1] = address_string_to_int(curTok);

    if ((tempReservedArea[0] >= memsize) || (tempReservedArea[1] >= memsize) || (tempReservedArea[0] < 0) || (tempReservedArea[1] < 0)) {

        raise_compiler_error(CompilerErrors::invalidMemoryAddress,
        "Couldn't reserve memory area as it is bigger than specified memory size ('memsize' instruction) or it is less than zero.",
        "... 'reserve ?"+std::to_string(tempReservedArea[0])+" ~ ?"+std::to_string(tempReservedArea[1])+"' ...");
    }

    reserved.push_back(tempReservedArea);

}