#include <iostream>
#include "style.hpp"

void print_it(std::string what, std::string where) {
	std::cout << REDD << "/!\\ " << where << RED << " :\n"<< what << RESET << std::endl;
}
