#include "common.hpp"
#include "optimise.hpp"

bool char_is_allowed(char c) {

    std::vector<char> allowed = {
        '+',
        '-',
        '<',
        '>',
        ',',
        '.',
        '[',
        ']',
        '\n'
    };

    bool ret = false;

    for (char ch: allowed) {
        if (ch == c) {
            ret = true;
        }
    }

    return ret;
}

bool match_neutralising_chars(char a, char b) {
    return (
        ((a == '<') && (b == '>')) ||
        ((a == '>') && (b == '<')) ||
        ((a == '+') && (b == '-')) ||
        ((a == '-') && (b == '+'))
    );
}

std::string remove_redundant_characters(std::string code) {

    std::string ret = "";

    for (char c: code) {
        if (char_is_allowed(c)) {
            ret += c;
        }
    }

    return ret;
}

std::string remove_neutralising_characters(std::string code) {

    std::string ret = "";
    bool modified = false;

    int stringSize = code.size();
    for (int i = 0; i < stringSize - 1; i++) {

        if (!match_neutralising_chars(code[i], code[i+1])) {
            ret += code[i];
        } else {
            i++;  // skip the next character because it's part of a pair
            modified = true;
        }
    }

    // Add the last character if it wasn't part of a neutralizing pair
    if (stringSize > 0 && !match_neutralising_chars(code[stringSize - 2], code[stringSize - 1])) {
        ret += code[stringSize - 1];
    }

    if (modified) {
        ret = remove_neutralising_characters(ret);
    }

    return ret;
}


std::string optimise_code(std::string code, int optimisationLevel) {

    std::string ret = code;

    if (optimisationLevel >= OptimisationLevel::OL_remove_neutralising_patterns) {
        ret = remove_neutralising_characters(ret);
    }

    if (optimisationLevel >= OptimisationLevel::OL_remove_redundant_characters) {
        ret = remove_redundant_characters(ret);
    }

    return ret;

}