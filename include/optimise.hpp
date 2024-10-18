#ifndef OPTIMISE_HPP
#define OPTIMISE_HPP

#include "common.hpp"

enum OptimisationLevel : int {
    OL_NONE,                            // no optimisation
    OL_remove_neutralising_patterns,    // remove -+ +- <> ><
    OL_remove_redundant_characters      // remove non-{+-<>,.[]\n} && OL_remove_neutralising_patterns
};

bool char_is_allowed(char c);
bool match_neutralising_chars(char a, char b);
std::string remove_redundant_characters(std::string code);
std::string remove_neutralising_characters(std::string code);
std::string optimise_code(std::string code, int optimisationLevel);


#endif