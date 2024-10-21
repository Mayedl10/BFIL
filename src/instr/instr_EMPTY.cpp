#include "compiler.hpp"
#include "common.hpp"

void Compiler::instr_empty () {

        tempStr = "";
        tempIntVect.clear();
        
        tPtr++;
        get_cur_tok();
        tempReservedArea[0] = address_string_to_int(curTok);

        tPtr+=2;
        get_cur_tok();
        tempReservedArea[1] = address_string_to_int(curTok);

        // this is just my "sanitize reserved area" script.
        // that's why I use tempReservedArea even though this 
        // sanitizes something else than a reserved area.
        tempStr += move_to(tempReservedArea[0]);
        tempStr += CS.setToZero;
        while (ptrPosition < tempReservedArea[1]) {
            tempStr += move_to(ptrPosition +1);
            tempStr += CS.setToZero;
        }

        out += tempStr;

}