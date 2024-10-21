#include "compiler.hpp"
#include "common.hpp"

void Compiler::instr_read () {

    tempStr = "";
    tempInt = 0;

    tPtr++;
    get_cur_tok();

    tempInt = address_string_to_int(curTok);

    if (reserved_overlap(reserved, {tempInt, tempInt})) {
        
        raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
        "Using a reserved address is not recommended!",              
        "... 'read " + curTok + "' ..." 
        );
    }

    tempStr += move_to(tempInt);
    tempStr += BFO.input;

    out += tempStr;

}