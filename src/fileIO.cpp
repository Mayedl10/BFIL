#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

std::string get_file_content_as_string(const std::string& filename) {

    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int write_to_file(std::string path, std::string fileExtension, std::string content, bool silent) {

    std::string filePath = path + fileExtension;
    std::ofstream file(filePath);

    if (file) {
        file << content;
        file.close();
    
        if (!silent) {
            std::cout << "File written successfully: " << filePath << std::endl;
        }
    
        return 0;

    } else {
        std::cerr << "Couldn't write to or open file: " << filePath << std::endl;
        return 1;
    }
    
}