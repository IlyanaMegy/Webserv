#include "../../inc/Webserv.hpp"

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

void checkToken(std::string &parametr)
{
	size_t pos = parametr.rfind(';');
	if (pos != parametr.size() - 1)
		throw std::runtime_error("Token is invalid");
	parametr.erase(pos);
}

int ft_stoi(std::string str)
{
    std::stringstream ss(str);
    if (str.length() > 10)
        throw std::exception();
    for (size_t i = 0; i < str.length(); ++i)
    {
        if(!isdigit(str[i]))
            throw std::exception();
    }
    int res;
    ss >> res;
    return (res);
}
