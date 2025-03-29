#ifndef PARSERTOOLS_HPP
#define PARSERTOOLS_HPP

#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <sstream>

int getTypePath(std::string const path);
int checkFile(std::string const path, int mode);
int isFileExistAndReadable(std::string const path, std::string const index);
size_t findStartServer(size_t start, std::string &content);
size_t findEndServer(size_t start, std::string &content);
void removeComments(std::string &content);
void removeWhiteSpace(std::string &content);
void checkToken(std::string &parametr);
int ft_stoi(std::string str);

#endif