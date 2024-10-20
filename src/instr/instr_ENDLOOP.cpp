#include "compiler.hpp"
#include "common.hpp"

void Compiler::instr_endLoop () {

    tempStr = "";
    tempInt = 0;

        /*
    steps (numbering continued from WNZ)

    3. pop loopingAddressesStack into tempInt (raise an error if the stack is empty)
    4. move_to tempInt
    5. ]

        */

    // .3
    if (loopingAddressesStack.empty()) {
        raise_compiler_error(CompilerErrors::unmatchedEndLoop, "Unmatched 'endLoop'. This means there are more 'endLoop' statements than 'whileNotZero' statements.");
    } else {
        tempInt = loopingAddressesStack.top();
        loopingAddressesStack.pop();    
    }

    // .4
    tempStr += move_to(tempInt);
    // .5
    tempStr += BFO.closeBr;

    out += tempStr;

}