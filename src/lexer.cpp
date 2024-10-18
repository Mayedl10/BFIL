#include <algorithm>
#include <iostream>
#include <vector>

#include "common.hpp"
#include "lexer.hpp"

static std::vector<Token> retV;

static std::string replaceSubChar(std::string master, char target, char newChar) {

    std::string ret = master;

    for (int ctr = 0; ctr<master.size(); ctr++) {

        if (ret[ctr] == target) {

            ret[ctr] = newChar;
        }
    }

    return ret;
}

std::string cleanSpaces(const std::string &input) {

    std::string result;
    bool previousSpace = false;  // Keeps track of the previous character being a space
    
    for (char c : input) {

        if (c == ' ') {

            if (!previousSpace) {
                result += c;
            }

            previousSpace = true;

        } else {

            result += c;
            previousSpace = false;
        }
    }
    
    return result;
}

static void addToken(std::string value, std::string type) {

    Token tempT;
    tempT.value = value;
    tempT.type = type;
    retV.push_back(tempT);
}

std::vector<std::string> tokenize(std::string code) {

    ReservedWords RW;

    code = " " + cleanSpaces(replaceSubChar(replaceSubChar(replaceSubChar(code, '\r', ' '), '\t', ' '), '\n', ' ')) + " ";

    std::vector<std::string> retV;
    std::string tempString;

    bool isComment = false;

    for (char c: code) {

        if (c == '#') {

            isComment = !isComment;

        } else if (c == ' ') {

            if (!(tempString.length() <= 0 || tempString == " ")) {

                retV.push_back(tempString);
                tempString = "";

            }

        } else {

            if (!isComment) {

                tempString += c;
            }
        }

    }
    
    return retV;
}
