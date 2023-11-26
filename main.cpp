#include <iostream>
#include <cstring>

#include "common.hpp"
#include "compiler.hpp"
#include "fileIO.hpp"
#include "lexer.hpp"

int main(int argc, char* argv[]) {

    // brainfuck ide: https://copy.sh/brainfuck/

    std::string filename = "file.bfil";
    std::string oFilename = "out";
    std::string oFileExt = "bf";
    std::string filePath = "";
    std::string oFilePath = "";

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
        return 0;
    }
    

    
    
    
    auto tokens = tokenize(fileContent);
    std::string compiledCode = compile(tokens);

    if (compiledCode != "do_not_output") {
        writeToFile(oFile, "", compiledCode);
    } else {
        std::cout << "------------------------------------------------------------------\n|Code will not be saved to file due to errors during compilation.|\n------------------------------------------------------------------" << std::endl;
    }

    

//    std::string file = "compare";
//    auto tokens = tokenize(getFileContentAsString("examples/"+file+".txt"));
//
//    std::string compiled_code = compile(tokens);
//
//
//    if (compiled_code != "do_not_output"){
//#ifdef COMPILER_DEBUG
//        std::cout << compiled_code << std::endl;
//#endif
//        writeToFile("output/"+file, ".bf", compiled_code);
//    } else {
//        std::cout << "------------------------------------------------------------------\n|Code will not be saved to file due to errors during compilation.|\n------------------------------------------------------------------" << std::endl;
//    }

    return 0;
}