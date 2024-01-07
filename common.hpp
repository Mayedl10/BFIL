#ifndef COMMON_HPP
#define COMMON_HPP

#include <array>
#include <iostream>
#include <stack>
#include <string>
#include <memory>
#include <vector>

struct ReservedWords {

    std::string RW_add          = "add";            // add
    std::string RW_alias        = "alias";          // so you can use names instead of addresses
    std::string RW_aout         = "aout";           // ascii output
    std::string RW_compare      = "compare";        // < > =
    std::string RW_copy         = "copy";           // copies values
    std::string RW_cout         = "cout";           // constant output
    std::string RW_decrement    = "decrement";      // decrement
    std::string RW_empty        = "empty";          // sets a range of addresses to 0
    std::string RW_endLoop      = "endLoop";        // }
    std::string RW_increment    = "increment";      // increment value
    std::string RW_inline       = "inline";         // keep stuff in output
    std::string RW_load         = "load";           // load value into address
    std::string RW_loads        = "loads";          // load, but with multiple values
    std::string RW_logic        = "logic";          // and or not xor
    std::string RW_memsize      = "memsize";        // specifies memory size
    std::string RW_read         = "read";           // ,
    std::string RW_reserve      = "reserve";        // reserves areas of code
    std::string RW_sub          = "sub";            // subtract
    std::string RW_vout         = "vout";           // value out
    std::string RW_wnz          = "whileNotZero";   // while (cur != 0) {

    std::string RW_LM_or        = "or";             // parameter for "compare"
    std::string RW_LM_and       = "and";            // ^
    std::string RW_LM_not       = "not";            // ^
};

struct Token {
    
    std::string value;
    std::string type;
};

std::vector<int> range (int a, int b);
bool stringContains (std::string master, std::string chars);
#endif