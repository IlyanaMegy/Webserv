#ifndef SIGNAL_HPP
# define SIGNAL_HPP

#include <signal.h>
#include <exception>

class Signal
{
	private:
		static void	_handleSIGINT(int signum);
		
	public:
		static void	setHandler(void);
		static bool	shouldStopServer;
};

#endif