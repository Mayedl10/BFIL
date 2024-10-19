#include "compiler.hpp"
#include "common.hpp"

void instr_load (
    std::string &out,
    std::string &tempStr,
    std::vector<int> &tempIntVect,
    std::array<int, 2> &tempReservedArea,
    int &tPtr,
    int &ptrPosition,
    int &tempInt,
    int &memsize
    ) {

    tPtr++;
    get_cur_tok();
    tempInt = address_string_to_int(curTok);

    if (reserved_overlap(reserved, {tempInt, tempInt})) {
        
        raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
        "Using a reserved address is not recommended!",              
        "... 'load " + curTok + " <- value' ..." 
        );
    }

    if (tempInt >= memsize || tempInt < 0) {

        raise_compiler_error(CompilerErrors::invalidMemoryAddress,
        "Couldn't load into address because it is bigger than specified memory size ('memsize' instruction) or it is less than zero.", 
        "... 'load ?"+std::to_string(tempInt)+" <- value' ...");
    }

    out += move_to(tempInt, ptrPosition);
    out += CS.setToZero;

    tPtr += 1;
    tPtr += 1;

    get_cur_tok();

    if (std::stoi(curTok) < 0 || std::stoi(curTok) > 255) {

        raise_compiler_error(CompilerErrors::invalidValueToLoad,
        "Couldn't load value because it is either less than zero or greater than 255. Only unsigned 8-bit integers (0-255) may be loaded.",
        "... 'load ?address <- "+curTok+"' ...");
    }

    out += add_n_chars(std::stoi(curTok), '+');

}