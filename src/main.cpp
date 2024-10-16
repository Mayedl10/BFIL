#include <iostream>
#include <cstring>

#include "common.hpp"
#include "compiler.hpp"
#include "fileIO.hpp"
#include "lexer.hpp"

void press_enter_to_continue() {
    std::cout << "Press enter to continue";
    std::cin.get();
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {

    std::string inputFile = "";
    std::string outputFile = "";
    bool keepWindowOpen = false;

    if (argc <= 1) {

        std::cout << "Please provide valid arguments." << std::endl;

        return 0;
    }

    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "-i") == 0) {
            inputFile = argv[++i];
        }

        if (strcmp(argv[i], "-o") == 0) {
            outputFile = argv[++i];
        }

        if (strcmp(argv[i], "-k") == 0) {
            keepWindowOpen = true;
        }

        if (strcmp(argv[i], "-O") == 0) {
            std::cout << "[Compiler flag -O is yet to be implemented. No optimisations will occur.]" << std::endl;
        }

        if (strcmp(argv[i], "-h") == 0) {
            std::cout
            << "-h help\n"
            << "-i input file path (eg. \"/foo/bar/test.bfil\")\n"
            << "-o output filename (eg. \"/foo/bar/test.bf\")\n"
            << "-k keep the window open after compilation is finished\n"
            << "-O optimise (not yet implemented, see issue #14)\n\n"
            << "If you encounter any bugs or have feature suggestions, please visit https://github.com/Mayedl10/BFIL/"
            << std::endl;
        }

    }


    if (outputFile == "") {
        std::cout << " [Please provide a file to write to]" << std::endl;
    }

    std::string fileContent;

    try
    {
        fileContent = getFileContentAsString(inputFile);
    }

    catch(const std::exception& e)
    {
        std::cout << "[Couldn't read input file]" << std::endl;
        return 1;
    }
    
    auto tokens = tokenize(fileContent);
    std::string compiledCode = compile(tokens);

    if (compiledCode != "do not output") {
        writeToFile(outputFile, "", compiledCode);
        
    } else {
        std::cout << "------------------------------------------------------------------\n|Code will not be saved to file due to errors during compilation.|\n------------------------------------------------------------------" << std::endl;
        
        if (keepWindowOpen) {
            
            press_enter_to_continue();

        }
        
        return 1;
    }


    return 0;
}