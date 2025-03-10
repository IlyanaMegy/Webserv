#include "CGI.hpp"

#include "Epoll.hpp"

CGI::CGI(void) : _arg(NULL), _env(NULL), _cpid(-1), _epoll(NULL), _hasSucceeded(false) {}

CGI::CGI(Epoll* epoll, std::string program, std::string cgi)
	: _program(program), _cgi(cgi), _arg(NULL), _env(NULL), _cpid(-1), _epoll(epoll), _hasSucceeded(false)
{
	(void) _arg;
	(void) _env;
	_launch();
}

CGI::~CGI(void) {}

int	CGI::getFd(void)
{
	return _pipeFd[READ_END];
}

bool	CGI::getHasSucceeded(void)
{
	return _hasSucceeded;
}

std::string	CGI::getOutput(void)
{
	return _output;
}

void	CGI::addOutput(std::string buffer)
{
	_output+=buffer;
}

void	CGI::wait(void)
{
	close(_pipeFd[READ_END]);
	
	int	wstatus;

	_hasSucceeded = waitpid(_cpid, &wstatus, 0) != - 1 && WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0;
}

void	CGI::_launch(void)
{
	if (pipe(_pipeFd) == -1)
		throw std::exception();
	if ((_cpid = fork()) == -1)
		throw std::exception();
	
	if (_cpid == 0) {
		close(_pipeFd[READ_END]);
		if (dup2(_pipeFd[WRITE_END], STDOUT_FILENO) == -1) {
			close(_pipeFd[WRITE_END]);
			throw std::exception();
		}
		close(_pipeFd[WRITE_END]);
		execve(_program.c_str(), (char*[]){(char *)_program.c_str(), (char *)_cgi.c_str(), NULL}, NULL);
		throw std::exception();
	}

	close(_pipeFd[WRITE_END]);

	_epoll->addFd(_pipeFd[READ_END], EPOLLIN, CGI_TIMEOUT);
}
