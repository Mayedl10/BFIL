#include "compiler.hpp"
#include "common.hpp"

void Compiler::instr_increment () {

    tempStr = "";
    tempInt = 0;

    tPtr++;
    get_cur_tok();

    tempInt = address_string_to_int(curTok);

    if (reserved_overlap(reserved, {tempInt, tempInt})) {
        
        raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
        "Using a reserved address is not recommended!",              
        "... 'increment " + curTok + "' ..." 
        );
    }

    tempStr += move_to(tempInt);
    tempStr += BFO.plus;

    out += tempStr;

}