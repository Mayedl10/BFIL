#include <iostream>
#include <cstring>

#include "common.hpp"
#include "compiler.hpp"
#include "fileIO.hpp"
#include "lexer.hpp"

#define pressEnterToContinue do { \
        std::cout << "Press enter to close this window."; \
        std::cin.get(); \
        std::cout << std::endl; \
    } while (0)

int main(int argc, char* argv[]) {

    std::string filename = "file.bfil";
    std::string oFilename = "out";
    std::string oFileExt = "bf";
    std::string filePath = "";
    std::string oFilePath = "";
    bool keepWindowOpen = false;

    if (argc <= 1) {

        std::cout << "Please provide valid arguments." << std::endl;

        return 0;
    }

    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "-f") == 0) {
            i++;
            filename = argv[i];
        }
        
        if (strcmp(argv[i], "-o") == 0) {

            i++;
            oFilename = argv[i];
        }
        
        if (strcmp(argv[i], "-O") == 0) {

            i++;
            oFileExt = argv[i];
        }
        
        if (strcmp(argv[i], "-p") == 0) {
            i++;
            filePath = argv[i];
        }
        
        if (strcmp(argv[i], "-P") == 0) {

            i++;
            oFilePath = argv[i];
        }

        if (strcmp(argv[i], "-k") == 0) {

            keepWindowOpen = true;

        }
    }


    if (oFilePath == "") {

        oFilePath = filePath;
    }

    std::string iFile = filePath + "\\" + filename;
    std::string oFile = oFilePath + "\\" + oFilename + "." + oFileExt;
    std::string fileContent;

    try
    {
        fileContent = getFileContentAsString(iFile);
    }

    catch(const std::exception& e)
    {
        return 1;
    }
    
    auto tokens = tokenize(fileContent);
    std::string compiledCode = compile(tokens);

    if (compiledCode != "do not output") {
        writeToFile(oFile, "", compiledCode);
        
    } else {
        std::cout << "------------------------------------------------------------------\n|Code will not be saved to file due to errors during compilation.|\n------------------------------------------------------------------" << std::endl;
        
        if (keepWindowOpen) {
            
            pressEnterToContinue;

        }
        
        return 1;
    }

    pressEnterToContinue;

    return 0;
}