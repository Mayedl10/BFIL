#include <iostream>
#include <cstring>
#include <string>

#include "common.hpp"
#include "compiler.hpp"
#include "fileIO.hpp"
#include "lexer.hpp"
#include "optimise.hpp"

static void press_enter_to_continue() {
    std::cout << "Press enter to continue";
    std::cin.get();
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {

    std::string inputFile = "";
    std::string outputFile = "";
    bool keepWindowOpen = false;
    int optimisationLevel = 0;
    bool displayWarnings = true;

    Compiler comp;

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
            optimisationLevel = std::stoi(argv[++i]);

        }

        if (strcmp(argv[i], "-h") == 0) {
            std::cout
            << "-h help\n"
            << "-i input file path (eg. \"/foo/bar/test.bfil\")\n"
            << "-o output filename (eg. \"/foo/bar/test.bf\")\n"
            << "-k keep the window open after compilation is finished\n"
            << "-O optimise (not yet implemented, see issue #14)\n\n"
            << "If you encounter any bugs or have feature suggestions, please visit https://github.com/Mayedl10/BFIL/ and open an issue"
            << std::endl;
        
        }

        if (strcmp(argv[i], "-c") == 0) {
            displayWarnings = false;
        
        }

    }


    if (outputFile == "" || inputFile == "") {
        std::cout << " [Please provide a file to write to/read from]" << std::endl;
    }

    std::string fileContent;

    try
    {
        fileContent = get_file_content_as_string(inputFile);
    }
    catch(const std::exception& e)
    {
        std::cout << "[Couldn't read input file]" << std::endl;
        return 1;
    }
    
    auto tokens = tokenize(fileContent);
    std::string compiledCode = comp.compile(tokens, displayWarnings);

    if (optimisationLevel) {
        compiledCode = optimise_code(compiledCode, optimisationLevel);

    }

#ifdef COMPILER_DEBUG
    std::cout << "compiledCode: " << compiledCode << std::endl; 
#endif

    

    if (comp.errorCount == 0) {
        write_to_file(outputFile, "", compiledCode);
        
    } else {
        std::cout << "------------------------------------------------------------------\n|Code will not be saved to file due to errors during compilation.|\n------------------------------------------------------------------" << std::endl;
        
        if (keepWindowOpen) {
            
            press_enter_to_continue();

        }
        
        return 1;
    }


    return 0;
}