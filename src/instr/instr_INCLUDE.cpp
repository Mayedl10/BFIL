#include "compiler.hpp"
#include "common.hpp"
#include "fileIO.hpp"
#include "lexer.hpp"

void Compiler::instr_include() {
    tPtr++;
    // skip everything as this instruction is handled before compile time
}

void Compiler::find_included_files() {
    for (int i = 0; i < (int)(Tokens.size()); i++) {
        if ((Tokens[i] == RW.RW_include)) {
            this->includedFiles.push_back(Tokens[i+1]);
        }
    }
}

void Compiler::include_files() {

    find_included_files();
    
    std::vector<std::string> previouslyIncluded = {};

    std::cout << "\n\n -- files --" << std::endl;
    for (auto y: includedFiles) {
        std::cout << y << std::endl;
    }

    std::cout << "\n\n -- folders --" << std::endl;
    for (auto y: linkerDirectories) {
        std::cout << y << std::endl;
    }


    std::vector<std::string> _tokens = {};
    bool foundFile = false;

    for (auto file: includedFiles) {
        foundFile = false;

        for (auto folder: linkerDirectories) {
            if (file_exists(folder + "/" + file) && !(vector_contains_string(previouslyIncluded, file))) {
                _tokens = tokenize(get_file_content_as_string(folder + "/" + file));
                generate_subroutines(_tokens);
                previouslyIncluded.push_back(file);
                foundFile = true;
            }
        }

        if (!foundFile) {

            std::string errorMessage = "Couldn't find file " + file + " in any of the following directories:";
            for (auto folder: linkerDirectories) {
                errorMessage += "\n" + folder;
            }

            raise_compiler_error(CompilerErrors::includedFileNotFound, errorMessage, "... include " + file + " ...");
        }

    }

}