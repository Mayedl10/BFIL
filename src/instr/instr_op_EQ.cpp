#include "compiler.hpp"
#include "common.hpp"

// github issue #30

/*

- grab t-1
- convert to integer
- grab t+1
- check if address/value
    - if address: copy.
    - if value: load.

*/

void Compiler::instr_op_EQ () {

    bool copyMode; // true if need to copy, false if need to load.

    tPtr--;
    get_cur_tok();  // on t-1

    tempIntVect = {};
    // no need to check if t-1 is an address, bc address_string_to_int would raise an error itself
    tempIntVect.push_back(address_string_to_int(curTok, RW.RW_prefix_ADDR));

    tPtr += 2;
    get_cur_tok();  // on t+1

    if ((curTok[0] == RW.RW_prefix_ADDR[0])) {
        // is address?

        tempInt = address_string_to_int(curTok);
        // we're still here, so is address.

        copyMode = true;
        tempIntVect.push_back(tempInt);

    } else if (is_valid_decimal(curTok)) {
        // is value.
        copyMode = false;
        tempIntVect.push_back(std::stoi(curTok));
    
    } else { // "Unexpected Token: Token nr. " + std::to_string(tPtr) + ".\nNote, that variables have been replaced with address strings (?n)."
        raise_compiler_error(CompilerErrors::typeError, "Expected const value or address/variable but got " + curTok + " instead. Token nr. " + std::to_string(tPtr) + ".\nNote, that variables have been replaced with address strings (?n).", "... = " + curTok + " ...");
    }

    if (copyMode) {
        copy_values(tempIntVect[1], tempIntVect[0], false);

    } else {
        load_const_value(tempIntVect[0], tempIntVect[1]);
    }

}