#include "CGI.hpp"

#include "Epoll.hpp"

CGI::CGI(void) : _arg(NULL), _env(NULL), _cpid(-1), _epoll(NULL), _hasSucceeded(false) {}

CGI::CGI(Epoll* epoll, std::string program, std::string cgi)
	: _program(program), _cgi(cgi), _arg(NULL), _env(NULL), _cpid(-1), _epoll(epoll), _hasSucceeded(false)
{
	_setEnv();
	_convertEnv();
	(void) _env;
	_launch();
}

CGI::~CGI(void)
{
	if (_envp) {
		for (std::size_t i = 0; i != _env.size(); i++)
			if (_envp[i])
				delete[] _envp[i];
		delete[] _envp;
	}

	close(_pipeFd[READ_END]);
}

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
		execve(_program.c_str(), (char*[]){(char *)_program.c_str(), (char *)_cgi.c_str(), NULL}, _envp);
		throw std::exception();
	}

	close(_pipeFd[WRITE_END]);

	_epoll->addFd(_pipeFd[READ_END], EPOLLIN, TIMEOUT);
}

void	CGI::_setEnv(void)
{

}

void	CGI::_convertEnv(void)
{
	_envp = new char*[_env.size() + 1];
	if (!_envp)
		throw std::exception();

	int	i = 0;
	for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); it++) {
		_fillVar(_envp + i, it->first, it->second);
		i++;
	}

	_envp[_env.size()] = NULL;

}

void	CGI::_fillVar(char** varp, std::string key, std::string value)
{
	std::string	var = key+"="+value;

	*varp = new char[var.length() + 1];
	if (!(*varp))
		throw std::exception();
	memcpy(*varp, var.c_str(), var.length());
	(*varp)[var.length()] = 0;
}


std::string	CGI::_itos(int value)
{
	std::string			res;
	std::ostringstream	stream;

	stream << value;
	res = stream.str();
	return res;
}
