#include "compiler.hpp"
#include "common.hpp"

void instr_sub (
    std::string &out,
    std::string &tempStr,
    std::vector<int> &tempIntVect,
    std::array<int, 2> &tempReservedArea,
    int &tPtr,
    int &ptrPosition
    ) {

    tempStr = "";
    tempIntVect.clear();    // forgetting this line. caused me to spend 4 HOURS TRYING TO FIGURE OUT WHY IT WASN'T WORKING! I EVEN FUCKING REWROTE THIS ENTIRE PART AAAA
    tempReservedArea = find_reserved(reserved, ORM.add); // reserved area with required size

    tPtr++;
    get_cur_tok();
    tempIntVect.push_back(address_string_to_int(curTok)); // param 1 ... idx[0]

    if (reserved_overlap(reserved, {tempIntVect[0], tempIntVect[0]})) {
        
        raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
        "Using a reserved address is not recommended!",              
        "... 'sub " + curTok + " <- address' ..." 
        );
    }

    tPtr+=2;
    get_cur_tok();
    tempIntVect.push_back(address_string_to_int(curTok)); // param 2 ... idx[1]

    if (reserved_overlap(reserved, {tempIntVect[1], tempIntVect[1]})) {
        
        raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
        "Using a reserved address is not recommended!",              
        "... 'add address <- " + curTok + "' ..." 
        );
    }

    // code gen

    // "sanitize" reserved area (just in case)
    tempStr += move_to(tempReservedArea[0]);
    tempStr += CS.setToZero;
    while (ptrPosition < tempReservedArea[1]) {
        tempStr += move_to(ptrPosition +1);
        tempStr += CS.setToZero;
    }

    // 0. move param1 to reserved[0]
    // 1. move param2 to reserved[1:2] (paste them twice there)
    // 2. move reserved[2] to param2 (so param2 can keep its value)
    // 3. add reserved[1] to reserved[0]
    // 4. move reserved[0] to param1
    // 5. "sanitize" reserved area again


    // .0 move param1 to reserved[0]
    tempStr += move_to(tempIntVect[0]);          // move_to param1
    tempStr += BFO.openBr;                      // [
    tempStr += BFO.minus;                       //  -   grab param1.value
    tempStr += move_to(tempReservedArea[0]);     //  <<< move_to reserved[0]
    tempStr += BFO.plus;                        //  +   place param1.value
    tempStr += move_to(tempIntVect[0]);          //  >>> move_to param1
    tempStr += BFO.closeBr;                     // ]

    // .1 move param2 to reserved[1:2] (paste them twice there)
    tempStr += move_to(tempIntVect[1]);          // move_to param2
    tempStr += BFO.openBr;                      // [
    tempStr += BFO.minus;                       //  -   grav param2.value
    tempStr += move_to(tempReservedArea[0]+1);   //  <<< move_to reserved[1]
    tempStr += BFO.plus;                        //  +   place param2.value
    tempStr += move_to(tempReservedArea[0]+2);   //  >   move_to reserved[2]
    tempStr += BFO.plus;                        //  +   place param2.value
    tempStr += move_to(tempIntVect[1]);          //  >>> move_to param2
    tempStr += BFO.closeBr;                     // ]

    // .2 move reserved[2] to param2 (so param2 can keep its value)
    tempStr += move_to(tempReservedArea[0]+2);   // move_to reserved[2]
    tempStr += BFO.openBr;                      // [
    tempStr += BFO.minus;                       //  -   grab reserved[2]
    tempStr += move_to(tempIntVect[1]);          //  >>> move_to param2
    tempStr += BFO.plus;                        //  +   place value
    tempStr += move_to(tempReservedArea[0]+2);   //  <<< move_to reserved[2]
    tempStr += BFO.closeBr;                     // ]

    // .3 add r[1] -> r[0]
    tempStr += move_to(tempReservedArea[0]+1);   // move_to reserved[1]
    tempStr += BFO.openBr;                      // [
    tempStr += BFO.minus;                       //  -   grab [1]
    tempStr += move_to(tempReservedArea[0]);     //  <<< move_to [0]
    tempStr += BFO.minus;                       //  -   sub [1] -> [0]      only line different from "add"
    tempStr += move_to(tempReservedArea[0]+1);   //  >>> move_to [1]
    tempStr += BFO.closeBr;                     // ]

    // .4 move reserved[0] to param1
    tempStr += move_to(tempReservedArea[0]);     // move_to reserved[0]
    tempStr += BFO.openBr;                      // [
    tempStr += BFO.minus;                       //  -   grab [0]
    tempStr += move_to(tempIntVect[0]);          //  >>> move_to param1
    tempStr += BFO.plus;                        //  +   place [0]
    tempStr += move_to(tempReservedArea[0]);     //  <<< move_to [0]
    tempStr += BFO.closeBr;                     // ]

    // .5 "sanitize" the reserved area again
    tempStr += move_to(tempReservedArea[0]);
    tempStr += CS.setToZero;
    while (ptrPosition < tempReservedArea[1]) {
        tempStr += move_to(ptrPosition +1);
        tempStr += CS.setToZero;
    }

    out += tempStr;

}