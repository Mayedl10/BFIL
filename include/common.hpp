#ifndef COMMON_HPP
#define COMMON_HPP

#include <array>
#include <iostream>
#include <stack>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <cctype>

// uncomment for debug mode
// #define COMPILER_DEBUG

struct ReservedWords {

    const std::string RW_add          = "add";            // add
    const std::string RW_alias        = "alias";          // so you can use names instead of addresses
    const std::string RW_aout         = "aout";           // ascii output
    const std::string RW_compare      = "compare";        // < > =
    const std::string RW_copy         = "copy";           // copies values
    const std::string RW_cout         = "cout";           // constant output
    const std::string RW_decrement    = "decrement";      // decrement
    const std::string RW_empty        = "empty";          // sets a range of addresses to 0
    const std::string RW_endLoop      = "endLoop";        // }
    const std::string RW_increment    = "increment";      // increment value
    const std::string RW_inline       = "inline";         // keep stuff in output
    const std::string RW_load         = "load";           // load value into address
    const std::string RW_loads        = "loads";          // load, but with multiple values
    const std::string RW_logic        = "logic";          // and or not xor
    const std::string RW_memsize      = "memsize";        // specifies memory size
    const std::string RW_read         = "read";           // ,
    const std::string RW_reserve      = "reserve";        // reserves areas of code
    const std::string RW_sub          = "sub";            // subtract
    const std::string RW_vout         = "vout";           // value out
    const std::string RW_wnz          = "whileNotZero";   // while (cur != 0) {

    const std::string RW_LM_or        = "or";             // parameter for "compare"
    const std::string RW_LM_and       = "and";            // ^
    const std::string RW_LM_not       = "not";            // ^
};

struct Token {
    
    std::string value;
    std::string type;
};

std::vector<int> range (int a, int b);
bool string_contains (std::string master, std::string chars);
#endif