#include "compiler.hpp"
#include "common.hpp"

void Compiler::instr_logic () {

    tempStr = "";
    tempReservedArea = {};
    tempIntVect.clear();
    tempInt = 0; 
    
    tPtr += 2;
    get_cur_tok();
    logicMode = curTok;

    tPtr--;
    get_cur_tok();

    tempIntVect.push_back(address_string_to_int(curTok));  // no matter the mode, [0] always contains the first parameter.

    if (logicMode == RW.RW_LM_and) {
        tempReservedArea = find_reserved(reserved, ORM.logic_and);

        tPtr += 2;
        get_cur_tok();
        tempIntVect.push_back(address_string_to_int(curTok)); // [1] is i2
        tPtr += 2;
        get_cur_tok();
        tempIntVect.push_back(address_string_to_int(curTok)); // [2] is output

        if (reserved_overlap(reserved, {tempIntVect.back(), tempIntVect.back()})) {
            
            raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
            "Using a reserved address is not recommended!",              
            "... 'logic address and address -> " + curTok + "' ..." 
            );
        }
        
        // and mode
        // steps:
            /*
            0. copy i1 to r[0:1]               // same as "or" //
            1. move r[1] to i1                 // same as "or" //
            2. copy i2 to r[1:2]               // same as "or" //
            3. move r[2] to i2                 // same as "or" //
            4. collapse r[1] to r[2]           // same as "or" //
            5. collapse r[0] to r[1]           // same as "or" //
            6. move r[1] to r[0]               // same as "or" //
            7. move r[2] to r[1]               // same as "or" //
            8. add r[1] -> r[0]                // same as "or" //
            9. do comparison magic with value 2
            10. move equality output to output
            11. sanitize
            */

        
        // .0   // same as "or" //
        tempStr += move_to(tempIntVect[0]);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempReservedArea[0]);
        tempStr +=BFO.plus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.plus;
        tempStr += move_to(tempIntVect[0]);
        tempStr += BFO.closeBr;

        // .1   // same as "or" //
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempIntVect[0]);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.closeBr;

        // .2   // same as "or" //
        tempStr += move_to(tempIntVect[1]);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr +=BFO.plus;
        tempStr += move_to(tempReservedArea[0]+2);
        tempStr += BFO.plus;
        tempStr += move_to(tempIntVect[1]);
        tempStr += BFO.closeBr;

        // .3   // same as "or" //
        tempStr += move_to(tempReservedArea[0]+2);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempIntVect[1]);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+2);
        tempStr += BFO.closeBr;

        // .4   // same as "or" //
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += CS.collapse_int;

        // .5   // same as "or" //
        tempStr += move_to(tempReservedArea[0]);
        tempStr += CS.collapse_int;

        // .6   // same as "or" //
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempReservedArea[0]);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.closeBr;

        // .7   // same as "or" //
        tempStr += move_to(tempReservedArea[0]+2);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+2);
        tempStr += BFO.closeBr;

        // .8   // same as "or" //
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempReservedArea[0]);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.closeBr;

        // .9 "Ich wollte meinen Stab zerbrechen..."
        // .9:1 move r[0] to r[3]
        // .9:2 load 2 into r[4]
        // .9:3 move_to r[0] and set ptrPosition to r[4]
        // .9:4 magic

        // .9:1
        tempStr += move_to(tempReservedArea[0]);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempReservedArea[0]+3);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]);
        tempStr += BFO.closeBr;

        // .9:2
        tempStr += move_to(tempReservedArea[0]+4);
        tempStr += CS.setToZero;
        tempStr += BFO.plus;
        tempStr += BFO.plus;

        // .9:3
        tempStr += move_to(tempReservedArea[0]);
        move_to(tempReservedArea[0]+4);

        // .9:4
        tempStr += CS.comparison_magic;

        // .10
        tempStr += move_to(tempIntVect[2]);
        tempStr += CS.setToZero;

        tempStr += move_to(tempReservedArea[0]);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempIntVect[2]);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]);
        tempStr += BFO.closeBr;

        // .11
        tempStr += move_to(tempReservedArea[0]);
        tempStr += CS.setToZero;
        while (ptrPosition < tempReservedArea[1]) {
            tempStr += move_to(ptrPosition +1);
            tempStr += CS.setToZero;
        }

    } else if (logicMode == RW.RW_LM_or) {

        tempReservedArea = find_reserved(reserved, ORM.logic_and);

        tPtr += 2;
        get_cur_tok();
        tempIntVect.push_back(address_string_to_int(curTok)); // [1] is i2
        tPtr += 2;
        get_cur_tok();
        tempIntVect.push_back(address_string_to_int(curTok)); // [2] is output

        if (reserved_overlap(reserved, {tempIntVect.back(), tempIntVect.back()})) {
            
            raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
            "Using a reserved address is not recommended!",              
            "... 'logic address or address -> " + curTok + "' ..." 
            );
        }

        // or mode
        // steps:
            /*
            
            0. copy i1 to r[0:1]               // same as "and" //
            1. move r[1] to i1                 // same as "and" //
            2. copy i2 to r[1:2]               // same as "and" //
            3. move r[2] to i2                 // same as "and" //
            4. collapse r[1] to r[2]           // same as "and" //
            5. collapse r[0] to r[1]           // same as "and" //
            6. move r[1] to r[0]               // same as "and" //
            7. move r[2] to r[1]               // same as "and" //
            8. add r[1] -> r[0]                // same as "and" //
            9. collapse r[0] to r[1]
            10. move r[1] to the zeroed output address
            */

        // .0   // same as "and" //
        tempStr += move_to(tempIntVect[0]);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempReservedArea[0]);
        tempStr +=BFO.plus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.plus;
        tempStr += move_to(tempIntVect[0]);
        tempStr += BFO.closeBr;

        // .1   // same as "and" //
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempIntVect[0]);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.closeBr;

        // .2   // same as "and" //
        tempStr += move_to(tempIntVect[1]);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr +=BFO.plus;
        tempStr += move_to(tempReservedArea[0]+2);
        tempStr += BFO.plus;
        tempStr += move_to(tempIntVect[1]);
        tempStr += BFO.closeBr;

        // .3   // same as "and" //
        tempStr += move_to(tempReservedArea[0]+2);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempIntVect[1]);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+2);
        tempStr += BFO.closeBr;

        // .4   // same as "and" //
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += CS.collapse_int;

        // .5   // same as "and" //
        tempStr += move_to(tempReservedArea[0]);
        tempStr += CS.collapse_int;

        // .6   // same as "and" //
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempReservedArea[0]);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.closeBr;

        // .7   // same as "and" //
        tempStr += move_to(tempReservedArea[0]+2);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+2);
        tempStr += BFO.closeBr;

        // .8   // same as "and" //
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempReservedArea[0]);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.closeBr;

        // .9
        tempStr += move_to(tempReservedArea[0]);
        tempStr += CS.collapse_int;

        // .10
        tempStr += move_to(tempIntVect[2]);
        tempStr += CS.setToZero;

        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempIntVect[2]);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.closeBr;

    } else if (logicMode == RW.RW_LM_not) {

        tempReservedArea = find_reserved(reserved, ORM.logic_not);
        // not mode

        tPtr+=3;
        get_cur_tok();
        tempIntVect.push_back(address_string_to_int(curTok));  // [1] = target

        if (reserved_overlap(reserved, {tempIntVect.back(), tempIntVect.back()})) {
            
            raise_compiler_warning(CompilerWarnings::accessingReservedAddress,
            "Using a reserved address is not recommended!",              
            "... 'logic address not -> " + curTok + "' ..." 
            );
        }

        // copy source to r[0:1]
        tempStr += move_to(tempIntVect[0]);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempReservedArea[0]);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.plus;
        tempStr += move_to(tempIntVect[0]);
        tempStr += BFO.closeBr;

        // move r[1] back to source
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempIntVect[0]);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.closeBr;

        // do the not-magic
        tempStr += move_to(tempReservedArea[0]);
        tempStr += CS.logic_not;
        tempStr += move_to(tempIntVect[1]);
        tempStr += CS.setToZero;

        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.openBr;
        tempStr += BFO.minus;
        tempStr += move_to(tempIntVect[1]);
        tempStr += BFO.plus;
        tempStr += move_to(tempReservedArea[0]+1);
        tempStr += BFO.closeBr;
        


    } else {

        raise_compiler_error(CompilerErrors::invalidLogicOperator,
        "Invalid operator for 'logic' statement.",
        "... 'logic ?address " + logicMode + "' ...");
    }

    out += tempStr;

}