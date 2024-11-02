#include "common.hpp"
#include "compiler.hpp"

/*
generate_subroutines()

unordered_map with: subroutine name (string), subroutine contents (vector<string>)

foreach token :
    if t == def:
        t++
        get name
        t++
        until t == return:
            add t to t vector
            t++
        new entry

*/

void Compiler::generate_subroutines() {

    tempStr = "";
    std::vector<std::string> subroutineContent = {};

    for (int i = 0; i < (int)(Tokens.size()); i++) {
        if (Tokens[i] == RW.RW_def) {

            i++;
            tempStr = Tokens[i];
            
            i++;
            while (Tokens[i] != RW.RW_return) {

                if (i >= (int)(Tokens.size())) {
                    raise_compiler_error(CompilerErrors::defWithoutReturn, "Subroutine without return statement.");
                    break;
                }

                if (Tokens[i] == RW.RW_def) {
                    raise_compiler_error(CompilerErrors::nestedSubroutineDefinition, "Cannot create subroutine inside subroutine.", "def keyowrd found inside " + tempStr);
                }

                subroutineContent.push_back(Tokens[i]);
                i++;
            }

            subroutineTokens.insert({tempStr, subroutineContent});
            subroutineContent.clear();
        }
    }
}

void Compiler::generate_subroutines(std::vector<std::string> &Tokens) {

    tempStr = "";
    std::vector<std::string> subroutineContent = {};

    for (int i = 0; i < (int)(Tokens.size()); i++) {
        if (Tokens[i] == RW.RW_def) {

            i++;
            tempStr = Tokens[i];
            
            i++;
            while (Tokens[i] != RW.RW_return) {

                if (i >= (int)(Tokens.size())) {
                    raise_compiler_error(CompilerErrors::defWithoutReturn, "Subroutine without return statement.");
                    break;
                }

                if (Tokens[i] == RW.RW_def) {
                    raise_compiler_error(CompilerErrors::nestedSubroutineDefinition, "Cannot create subroutine inside subroutine.", "def keyowrd found inside " + tempStr);
                }

                subroutineContent.push_back(Tokens[i]);
                i++;
            }

            subroutineTokens.insert({tempStr, subroutineContent});
            subroutineContent.clear();
        }
    }
}

bool Compiler::subroutine_exists(std::string name) {
    // std::pair<std::string, std::vector<std::string>>
    for (auto p: subroutineTokens) {
        if (p.first == name) {
            return true;
        }
    }
    return false;
}

/*
insert_subroutine_tokens()

foreach token
    if call
        insert tokens
    elif def
        skip until return. if no return, error
    elif return
        error
    else
        add token

the instr_ functions for call, def, and return tokens should never be called. if they ARE called, error.
*/

void Compiler::insert_subroutine_tokens() {

    // inserts tokens from subroutine, leave all call, def, return tokens out

    std::vector<std::string> newTokens = {};

    for (int i = 0; i < (int)(Tokens.size()); i++) {

        if (Tokens[i] == RW.RW_call) {
            i++;
            
            tempStr = Tokens[i];

            if (subroutine_exists(Tokens[i])) {
                for (std::string t: subroutineTokens[Tokens[i]]) {
                    newTokens.push_back(t);
                }

            } else {
                raise_compiler_error(CompilerErrors::invalidSubroutineIdentifier, "Subroutine " + Tokens[i] + " is undefined.", "... " + RW.RW_call + " " + Tokens[i] + " ...");
            }
        
        } else if (Tokens[i] == RW.RW_def) {

            while (Tokens[i] != RW.RW_return) {
                if (i >= Tokens.size()) {
                    raise_compiler_error(CompilerErrors::defWithoutReturn, "Subroutine without return statement.");
                    break;
                }
                i++;
            }
                    
        } else if (Tokens[i] == RW.RW_return) {
            raise_compiler_error(CompilerErrors::returnWithoutDef, "Attempted to return without subroutine to return from.", "... " + Tokens[i] + " ...");
        
        } else {
            newTokens.push_back(Tokens[i]);
        }

    }

    Tokens = newTokens;

}