#include "compiler.hpp"
#include "common.hpp"

void instr_vout (
    std::string &out,
    std::string &tempStr,
    std::vector<int> &tempIntVect,
    std::array<int, 2> &tempReservedArea,
    int &tPtr,
    int &ptrPosition,
    int &tempInt,
    int &memsize
    ) {

    tempStr = "";
    tempInt = 0;
    tempIntVect.clear();
    tempReservedArea = {};

    // steps:
        /*
        0. get the correct memory addresses into tempIntVect
        1. find a big enough reserved area and sanitize it
        2. for each value in tempIntVect do:
            2.0 copy value from addres to r[0:1]
            2.1 move r[1] back to address
            2.2 move to r[0]
            2.3 do the magic
            2.4 sanitize the reserved area
        */

    // .0
    tPtr++;
    get_cur_tok();
    tempInt = std::stoi(curTok);    // now contains the amount of parameters

    // get the addresses into tempIntVect
    for (int i = 0; i < tempInt; i++) {
        tPtr++;
        get_cur_tok();
        tempIntVect.push_back(address_string_to_int(curTok));
    }

    // .1
    tempReservedArea = find_reserved(reserved, ORM.vout);

    // sanitize
    tempStr += move_to(tempReservedArea[0]);
    tempStr += CS.setToZero;
    while (ptrPosition < tempReservedArea[1]) {
        tempStr += move_to(ptrPosition +1);
        tempStr += CS.setToZero;
    }

    // .2
    //get address of separating char
    tPtr+=2;
    get_cur_tok();
    bool doSeparate;
    if (address_string_to_int(curTok) < 0 || address_string_to_int(curTok) > memsize) {
        doSeparate = false;
    } else {
        doSeparate = true;
    }
    int separatorAddress = address_string_to_int(curTok);

    // do .2::[0:4]
    for (int i = 0; i < tempIntVect.size(); i++) {
        int address = tempIntVect[i];                
        //.2::0 copy value
        tempStr += move_to(address);                 // move_to address
        tempStr += BFO.openBr;                      // [
        tempStr += BFO.minus;                       //  -   grab value
        tempStr += move_to(tempReservedArea[0]);     //  <<< move_to r[0]
        tempStr += BFO.plus;                        //  +   paste @r[0]
        tempStr += move_to(tempReservedArea[0]+1);   //  >   move_to r[1]
        tempStr += BFO.plus;                        //  +   paste @r[1]
        tempStr += move_to(address);                 //  >>> move_to address
        tempStr += BFO.closeBr;                     // ]

        // .2::1 r[1] -> address
        tempStr += move_to(tempReservedArea[0]+1);   // move_to r[1]
        tempStr += BFO.openBr;                      // [
        tempStr += BFO.minus;                       //  -   grab value
        tempStr += move_to(address);                 //  >>> move_to address
        tempStr += BFO.plus;                        //  +   paste @address
        tempStr += move_to(tempReservedArea[0]+1);   //  <<< move_to r[1]
        tempStr += BFO.closeBr;                     // ]

        // .2::2 move_to r[0]
        tempStr += move_to(tempReservedArea[0]);

        // .2::3 do da magic
        tempStr += CS.numOut;

        // .2::4 sanitize and insert spaces between numbers
        tempStr += move_to(tempReservedArea[0]);
        tempStr += CS.setToZero;
        while (ptrPosition < tempReservedArea[1]) {
            tempStr += move_to(ptrPosition +1);
            tempStr += CS.setToZero;
        }

        if (doSeparate && (i+1 < tempIntVect.size())) {
            tempStr += move_to(separatorAddress);
            tempStr += BFO.asciiOut;
        }

    }

    out += tempStr;

}