#include "../../inc/config/ParserTools.hpp"
#include "../../inc/style.hpp"
#include <iostream>

int getTypePath(std::string const path) {
	struct stat buff;
	std::string modified_path = path;

    if (!modified_path.empty() && modified_path[modified_path.size() - 1] == ';')
		modified_path.erase(modified_path.size() - 1);

	int res = stat(modified_path.c_str(), &buff);
	if (!res) {
		if (buff.st_mode & S_IFREG)
			return 1;
		else if (buff.st_mode & S_IFDIR)
			return 2;
		return 3;
	}
	return -1;
}

int checkFile(std::string const path, int mode) {
	return (access(path.c_str(), mode));
}

int isFileExistAndReadable(std::string const path, std::string const index) {
	if (getTypePath(index) == 1 && checkFile(index, 4) == 0) return (0);
	if (getTypePath(path + index) == 1 && checkFile(path + index, 4) == 0)
		return (0);
	return (-1);
}

size_t findStartServer(size_t start, std::string &content) {
	size_t i;

	for (i = start; content[i]; i++) {
		if (content[i] == 's') break;
		if (!isspace(content[i]))
			throw std::runtime_error("Error: Wrong character out of server scope{}");
	}
	if (!content[i]) return (start);
	if (content.compare(i, 6, "server") != 0)
		throw std::runtime_error("Error: Wrong character out of server scope{}");
	i += 6;
	while (content[i] && isspace(content[i])) i++;
	if (content[i] == '{') return (i);
	throw std::runtime_error("Error: Wrong character out of server scope{}");
}

size_t findEndServer(size_t start, std::string &content) {
	size_t i;
	size_t scope;

	scope = 0;
	for (i = start + 1; content[i]; i++) {
		if (content[i] == '{') scope++;
		if (content[i] == '}') {
			if (!scope) return (i);
			scope--;
		}
	}
	return (start);
}

void removeComments(std::string &content) {
	size_t pos;

	pos = content.find('#');
	while (pos != std::string::npos) {
		size_t pos_end;
		pos_end = content.find('\n', pos);
		content.erase(pos, pos_end - pos);
		pos = content.find('#');
	}
}

void removeWhiteSpace(std::string &content) {
	size_t i = 0;

	while (content[i] && isspace(content[i])) i++;
	content = content.substr(i);
	i = content.length() - 1;
	while (i > 0 && isspace(content[i])) i--;
	content = content.substr(0, i + 1);
}

void checkToken(std::string &param)
{
	if (param.empty() || param[param.size() - 1] != ';')
		throw std::runtime_error("Token is invalid : " + param);
}

int ft_stoi(std::string str)
{
    std::stringstream ss(str);
    if (str.length() > 10)
		throw std::runtime_error("Token is invalid : " + str);
    for (size_t i = 0; i < str.length(); ++i)
        if(!isdigit(str[i]))
			throw std::runtime_error("Token is invalid : " + str);
    int res;
    ss >> res;
    return (res);
}
