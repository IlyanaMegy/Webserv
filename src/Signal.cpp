#include "Signal.hpp"

bool	Signal::shouldStopServer = false;

void	Signal::setHandler(void)
{
	if (signal(SIGINT, &_handleSIGINT) == SIG_ERR || signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		throw std::exception();
}

void	Signal::_handleSIGINT(int signum)
{
	(void) signum;
	Signal::shouldStopServer = true;
}