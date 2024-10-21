#include "compiler.hpp"
#include "common.hpp"

void Compiler::instr_loads () {

    tempStr = "";
    tempInt = 0;
    tempIntVect.clear();

    tPtr++;
    get_cur_tok();

    tempInt = address_string_to_int(curTok);
    tempStr += move_to(tempInt);

    if (tempInt < 0 || tempInt >= memsize) {
        raise_compiler_error(CompilerErrors::invalidMemoryAddress,
        "Couldn't load into address because it is bigger than specified memory size ('memsize' instruction) or it is less than zero.", 
        "... 'loads ?"+std::to_string(tempInt)+"...");
    }

    if (reserved_overlap(reserved, {tempInt, tempInt})) {
        
        raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
        "Using a reserved address is not recommended!",              
        "... 'loads " + curTok + " ~ address : values' ..."
        );
    }

    tPtr+=2;
    get_cur_tok();
    tempInt = std::stoi(curTok);
    tPtr++;

    for (int i = 0; i < tempInt; i++) {

        tPtr++;
        get_cur_tok();
        tempIntVect.push_back(std::stoi(curTok));
    }

    for (int y: tempIntVect) {

        tempStr += CS.setToZero;
        tempStr += add_n_chars(y, '+');
        tempStr += move_to(ptrPosition+1);

        if ((y > 255) || (y < 0)) {
            raise_compiler_error(CompilerErrors::invalidValueToLoad,
            "Couldn't load value because it is either less than zero or greater than 255. Only unsigned 8-bit integers (0-255) may be loaded.",
            "... 'loads "+curTok+"' ...");
        }

        if (reserved_overlap(reserved, {y, y})) {
            
            raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
            "Using a reserved address is not recommended!",              
            "... 'loads address ~ address : values' ...\nA loads statement is trying to write to address ?" + std::to_string(y)
            );
        }

    }

    out += tempStr;

}