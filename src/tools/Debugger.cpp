#include "../../inc/Webserv.hpp"

Debugger::Debugger(void) {}
Debugger::~Debugger(void) {}
Debugger &Debugger::operator=(const Debugger &src) {
	(void)src;
	return (*this);
}

void Debugger::printString(const std::string &msg, e_LogLevel lvl) {
	// if (ENABLE_LOGGING == 0) return ;
	std::string prefix = "";
	switch (lvl) {
		case INFO:
			std::cout << GOLD << "[INFO]:" << RESET << msg << std::endl;
			break;
		case WARNING:
			std::cout << PEAC << "[WARNING]:" << RESET << msg << std::endl;
			break;
		case DEBUG:
			std::cout << TURQ << "[DEBUG]:" << RESET << msg << std::endl;
			break;
		case ERROR:
			std::cerr << CORL << "[ERROR]:" << RESET << msg << std::endl;
			break;
		default:
			std::cout << GRY1 << "[DEBUG]:" << RESET << msg << std::endl;
	}
}
