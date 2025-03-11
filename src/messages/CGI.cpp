#include "CGI.hpp"

#include "Epoll.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "Socket.hpp"

CGI::CGI(void) : _envp(NULL), _cpid(-1), _epoll(NULL), _client(NULL), _request(NULL), _hasSucceeded(false) {}

CGI::CGI(Epoll* epoll, Request* request, std::string program, std::string cgi)
	: _program(program), _cgi(cgi), _envp(NULL), _cpid(-1), _epoll(epoll), _client(request->getClient()), _request(request),_hasSucceeded(false)
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
	if (_request->getBodyLength())
		_env["CONTENT_LENGTH"] = _request->getBodyLength();
	
	if (_request->getFields().find("content-type") != _request->getFields().end())
		_env["CONTENT_TYPE"] = _request->getFields()["content-type"][0];

	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	
	_env["PATH_INFO"] = _cgi;
	
	_env["QUERY_STRING"] = _request->getQuery();
	
	_env["REMOTE_ADDR"] = std::string(inet_ntoa(_client->getSocket().getAddr().sin_addr));
	
	_env["REQUEST_METHOD"] = _request->getMethod() == Request::GET ? "GET" : "POST";
	
	_env["SCRIPT_NAME"] = _cgi;
	
	_env["SERVER_PORT"] = _itos(_request->getServer()->getPort());
	
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	
	_env["SERVER_SOFTWARE"] = "webserv";

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
