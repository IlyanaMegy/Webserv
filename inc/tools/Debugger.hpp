#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#define USE_DEBUGGER Debugger debug = debug.getInstance();

#include <iostream>

#include "Singletons.hpp"
#include "style.hpp"

enum e_LogLevel { INFO = 'I', WARNING = 'W', DEBUG = 'D', ERROR = 'E' };

class Debugger : public Singleton<Debugger>
{
   private:
	void printString(const std::string &msg, e_LogLevel lvl);

   public:
	Debugger(void);
	Debugger &operator=(const Debugger &src);
	~Debugger(void);

	// void info(const std::string &msg);
	// void warning(const std::string &msg);
	// void debug(const std::string &msg);
	// void error(const std::string &msg);
};

#endif
