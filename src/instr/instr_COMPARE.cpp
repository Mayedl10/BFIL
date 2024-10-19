#include "compiler.hpp"
#include "common.hpp"

void instr_compare (
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
    std::string comparisonMode = "";

    tPtr++;
    get_cur_tok();
    tempIntVect.push_back(address_string_to_int(curTok)); // get first input address into [0]

    tPtr++;
    get_cur_tok();
    comparisonMode = curTok;    // either =, <, or >

    tPtr++;
    get_cur_tok();
    tempIntVect.push_back(address_string_to_int(curTok)); // get second input address into [1]

    tPtr+=2;
    get_cur_tok();
    tempIntVect.push_back(address_string_to_int(curTok)); // get output address into [2]

    // memory layout for "compare": o1  o2  o3  i1  i2  r1
    //                              [0] [1] [2] [3] [4] [5]
    // steps: 
        /*
        0. find reserved area and sanitize
        1. copy [0] to r[3:4]
        2. move r[4] to [1]
        3. copy [1] to r[4:5]
        4. move r[5] to [2]
        5. move_to r[4] (this is where the pointer is after the comparison. I don't want to loose track of that thing)
        6. insert magic.
        7. move to the output address and set it to zero (to not interfere with 8.) 
        8. depending on "comparisonMode", move to either r[0], r[1] or r[2] and move that value to the output parameter.
        9. sanitize r
        */

    // .0
    tempReservedArea = find_reserved(reserved, ORM.compare);

    // sanitize
    tempStr += move_to(tempReservedArea[0]);
    tempStr += CS.setToZero;
    while (ptrPosition < tempReservedArea[1]) {
        tempStr += move_to(ptrPosition +1);
        tempStr += CS.setToZero;
    }

    // .1 copy [0] to r[3:4]
    tempStr += move_to(tempIntVect[0]);              // move_to input 1
    tempStr += BFO.openBr;                          // [
    tempStr += BFO.minus;                           //  -   grab value
    tempStr += move_to(tempReservedArea[0]+3);       //  <<< move_to r[3]
    tempStr += BFO.plus;                            //  +   place @r[3]
    tempStr += move_to(tempReservedArea[0]+4);   	//  >   move_to r[4]
    tempStr += BFO.plus;                            //  +   place @r[4]
    tempStr += move_to(tempIntVect[0]);              //  >>> move_to input 1
    tempStr += BFO.closeBr;                         // ]

    // .2 move r[4] to [1]
    tempStr += move_to(tempReservedArea[0]+4);       // move_to r[4]
    tempStr += BFO.openBr;                          // [
    tempStr += BFO.minus;                           //  -   grab value
    tempStr += move_to(tempIntVect[0]);              //  >>> move_to input 1
    tempStr += BFO.plus;                            //  +   place @input 1
    tempStr += move_to(tempReservedArea[0]+4);       //  <<< move_to r[4]
    tempStr += BFO.closeBr;                         // ]

    // .3 copy [1] to r[4:5]
    tempStr += move_to(tempIntVect[1]);              // move_to input 2
    tempStr += BFO.openBr;                          // [
    tempStr += BFO.minus;                           //  -   grab value
    tempStr += move_to(tempReservedArea[0]+4);       //  <<< move_to r[4]
    tempStr += BFO.plus;                            //  +   place @r[4]
    tempStr += move_to(tempReservedArea[0]+5);   	//  >   move_to r[5]
    tempStr += BFO.plus;                            //  +   place @r[5]
    tempStr += move_to(tempIntVect[1]);              //  >>> move_to input 2
    tempStr += BFO.closeBr;                         // ]

    // .4 move r[5] to [2]
    tempStr += move_to(tempReservedArea[0]+5);       // move_to r[5]
    tempStr += BFO.openBr;                          // [
    tempStr += BFO.minus;                           //  -   grab value
    tempStr += move_to(tempIntVect[1]);              //  >>> move_to input 1
    tempStr += BFO.plus;                            //  +   place @input 1
    tempStr += move_to(tempReservedArea[0]+5);       //  <<< move_to r[5]
    tempStr += BFO.closeBr;  

    // .5
    tempStr += move_to(tempReservedArea[0]);         // this is where the pointer needs to be

    // .6 magic
    tempStr += CS.comparison_magic;                 // dangerous magic.
    move_to(tempReservedArea[0]+4);


    // .7 move to the output address and set it to zero (to not interfere with 8.) 
    tempStr += move_to(tempIntVect[2]);
    tempStr += CS.setToZero;

    // .8  depending on "comparisonMode", move to either r[0], r[1] or r[2] and move that value to the output parameter.
    if (comparisonMode == "=") {

        tempStr += move_to(tempReservedArea[0]);

    } else if (comparisonMode == ">") {

        tempStr += move_to(tempReservedArea[0]+1);

    } else if (comparisonMode == "<") {

        tempStr += move_to(tempReservedArea[0]+2);

    } else {
        
        raise_compiler_error(CompilerErrors::invalidComparisonOperator, "Invalid comparison operator.", "... 'compare ?address " + comparisonMode + " ?address -> ?address' ...");
    }

    tempStr += BFO.openBr;                          // [
    tempStr += BFO.minus;                           //  -
    tempStr += move_to(tempIntVect[2]);              //  >>>
    tempStr += BFO.plus;                            //  +

    if (comparisonMode == "=") {                    //  <<<

        tempStr += move_to(tempReservedArea[0]);

    } else if (comparisonMode == ">") {

        tempStr += move_to(tempReservedArea[0]+1);

    } else if (comparisonMode == "<") {

        tempStr += move_to(tempReservedArea[0]+2);

    } else {

        raise_compiler_error(CompilerErrors::invalidComparisonOperator, "Invalid comparison operator.", "... 'compare ?address " + comparisonMode + " ?address -> ?address' ...");
    }

    tempStr += BFO.closeBr;

    // .9
    tempStr += move_to(tempReservedArea[0]);
    tempStr += CS.setToZero;
    while (ptrPosition < tempReservedArea[1]) {
        tempStr += move_to(ptrPosition +1);
        tempStr += CS.setToZero;
    }


        /*
        So will ich meinen Stab zerbrechen,
        ihn etliche Klafter tief in die Erde vergraben,
        und tiefer als jemals ein Senkbley fiel,
        mein Zauberbuch im Meer versenken.
        -William Shakespeare: Der Sturm, Akt 5, Szene 1

        I'll break my staff,
        Bury it certain fathoms in the earth,
        And deeper than did ever plummet sound
        I'll drown my book.
        -William Shakespeare: The tempest, act 5, scene 1          
        */

    // I added this as a joke because of the incredible fuckery I make my compiler do here. I give over the control over the pointer to the comparison algorithm and have to trust that it always ends up at the same position.


    out += tempStr;

}