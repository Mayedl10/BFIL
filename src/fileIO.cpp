#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

std::string getFileContentAsString(const std::string& filename) {

    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int writeToFile(std::string path, std::string fileExtension, std::string content) {

    std::string filePath = path + fileExtension;
    std::ofstream file(filePath);

    if (file) {
        file << content;
        file.close();
        std::cout << "File written successfully: " << filePath << std::endl;
        return 0;

    } else {
        std::cerr << "Couldn't write to or open file: " << filePath << std::endl;
        return 1;
    }
    
}