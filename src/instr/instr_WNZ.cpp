#include "compiler.hpp"
#include "common.hpp"

void instr_wnz (
    std::string &out,
    std::string &tempStr,
    std::vector<int> &tempIntVect,
    std::array<int, 2> &tempReservedArea,
    int &tPtr,
    int &ptrPosition,
    std::stack<int> &loopingAddressesStack
    ) {

    tempStr = "";

        /*

    steps
    
    0. grab the target and push it to loopingAddressesStack
    1. move to loopingAddressesStack.top()
    2. [
    
    the rest is done in the "endLoop" part            
        */

    tPtr++;
    get_cur_tok();
    loopingAddressesStack.push(address_string_to_int(curTok)); // .0

    tempStr += move_to(loopingAddressesStack.top());             // .1
    tempStr += BFO.openBr;                                      // .2

    out += tempStr;

}