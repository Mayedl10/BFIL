#ifndef FILEIO_HPP
#define FILEIO_HPP

#include <string>

bool file_exists(const std::string path);
std::string get_file_content_as_string(const std::string& filename);
int write_to_file(std::string path, std::string fileExtension, std::string content, bool silent);

#endif