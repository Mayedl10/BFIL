#ifndef FILEIO_HPP
#define FILEIO_HPP

#include <string>

std::string getFileContentAsString(const std::string& filename);
int writeToFile(std::string path, std::string fileExtension, std::string content);

#endif