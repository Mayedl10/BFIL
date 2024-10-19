#include "compiler.hpp"
#include "common.hpp"

void instr_copy (
    std::string &out,
    std::string &tempStr,
    std::vector<int> &tempIntVect,
    std::array<int, 2> &tempReservedArea,
    int &tPtr,
    int &ptrPosition,
    int &tempInt
    ) {

    tempStr = "";
    tempInt = 0;
    tempIntVect.clear();
    tempReservedArea = {};

    tPtr++;
    get_cur_tok();
    tempIntVect.push_back(address_string_to_int(curTok)); // get target

    if (reserved_overlap(reserved, {tempIntVect[0], tempIntVect[0]})) {
        
        raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
        "Using a reserved address is not recommended!",              
        "... 'copy " + curTok + " <- address' ..." 
        );
    }

    tPtr+=2;
    get_cur_tok();
    tempIntVect.push_back(address_string_to_int(curTok)); // get source

    if (reserved_overlap(reserved, {tempIntVect[1], tempIntVect[1]})) {
        
        raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
        "Using a reserved address is not recommended!",              
        "... 'copy address <- " + curTok + "' ..." 
        );
    }

    // tempIntVect[0]...target ; [1]...source

        /*
    steps:

    0. find and sanitize reserved area
    1. copy source and paste it twice in r[0:1]
    2. move r[0] to source
    3. move r[1] to target
    4. sanitize reserved area

        */

    // .0
    tempReservedArea = find_reserved(reserved, ORM.copy);

    // sanitize
    tempStr += move_to(tempReservedArea[0]);
    tempStr += CS.setToZero;
    while (ptrPosition < tempReservedArea[1]) {
        tempStr += move_to(ptrPosition +1);
        tempStr += CS.setToZero;
    }

    // .1
    tempStr += move_to(tempIntVect[1]);          // move_to source
    tempStr += BFO.openBr;                      // [
    tempStr += BFO.minus;                       //  -   grab value
    tempStr += move_to(tempReservedArea[0]);     //  <<< move_to r[0]
    tempStr += BFO.plus;                        //  +   paste @r[0]
    tempStr += move_to(tempReservedArea[0]+1);   //  >   move_to r[1]
    tempStr += BFO.plus;                        //  +   paste @r[1]
    tempStr += move_to(tempIntVect[1]);          //  >>> move_to source
    tempStr += BFO.closeBr;                     // ]

    // .2

    tempStr += move_to(tempIntVect[1]);
    tempStr += CS.setToZero;

    tempStr += move_to(tempReservedArea[0]);     // move_to r[0]
    tempStr += BFO.openBr;                      // [
    tempStr += BFO.minus;                       //  -   grab value
    tempStr += move_to(tempIntVect[1]);          //  >>> move_to source
    tempStr += BFO.plus;                        //  +   paste value
    tempStr += move_to(tempReservedArea[0]);     //  <<< move_to r[0]
    tempStr += BFO.closeBr;                     // ]

    // .3
    tempStr += move_to(tempIntVect[0]);
    tempStr += CS.setToZero;

    tempStr += move_to(tempReservedArea[0]+1);   // move_to r[1]
    tempStr += BFO.openBr;                      // [
    tempStr += BFO.minus;                       //  -   grab value
    tempStr += move_to(tempIntVect[0]);          //  >>> move_to target
    tempStr += BFO.plus;                        //  +   paste value
    tempStr += move_to(tempReservedArea[0]+1);   //  <<< move_to r[1]
    tempStr += BFO.closeBr;                     // ]

    // .4
    tempStr += move_to(tempReservedArea[0]);
    tempStr += CS.setToZero;
    while (ptrPosition < tempReservedArea[1]) {
        tempStr += move_to(ptrPosition +1);
        tempStr += CS.setToZero;
    }

    out += tempStr;

}