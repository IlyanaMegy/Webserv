#include "CGI.hpp"

#include "Epoll.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "ServerConf.hpp"
#include "Socket.hpp"

CGI::CGI(void) : _envp(NULL), _cpid(-1), _epoll(NULL), _client(NULL), _request(NULL), _hasSucceeded(false), _statusCode("200") {}

CGI::CGI(Epoll* epoll, Request* request, std::string program, std::string cgi)
	: _program(program), _cgi(cgi), _envp(NULL), _cpid(-1), _epoll(epoll), _client(request->getClient()), _request(request), _hasSucceeded(false), _wasWaitedFor(false), _statusCode("200")
{
	_initPipes();
	_setEnv();
	_convertEnv();
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

	if (_pipeFdOut[READ_END] != CLOSED)
		close(_pipeFdOut[READ_END]);
	if (_pipeFdIn[WRITE_END] != CLOSED)
		close(_pipeFdIn[WRITE_END]);

	if (!_wasWaitedFor) {
		kill(_cpid, SIGKILL);
		waitpid(_cpid, NULL, 0);
	}
}

void	CGI::_initPipes(void)
{
	_pipeFdOut[READ_END] = CLOSED;
	_pipeFdOut[WRITE_END] = CLOSED;
	_pipeFdIn[READ_END] = CLOSED;
	_pipeFdIn[WRITE_END] = CLOSED;
}

int	CGI::getReadFd(void)
{
	return _pipeFdOut[READ_END];
}

int	CGI::getWriteFd(void)
{
	return _pipeFdIn[WRITE_END];
}


bool	CGI::getHasSucceeded(void)
{
	return _hasSucceeded;
}

std::string	CGI::getStatusCode(void)
{
	return _statusCode;
}

std::string	CGI::getReasonMessage(void)
{
	return _reasonMessage;
}

std::map< std::string, std::vector<std::string> >	CGI::getFields(void)
{
	return _fields;
}

std::string	CGI::getBody(void)
{
	return _body;
}

void	CGI::closeWriteFd(void)
{
	close(_pipeFdIn[WRITE_END]);
	_pipeFdIn[WRITE_END] = CLOSED;
}

void	CGI::addOutput(std::string buffer)
{
	_output+=buffer;
}

void	CGI::wait(void)
{
	int	wstatus;

	_hasSucceeded = waitpid(_cpid, &wstatus, 0) != - 1 && WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0;
	_wasWaitedFor = true;
}

void	CGI::parse(void)
{
	std::string::size_type	lfLfPos;

	lfLfPos = _output.find("\n\n");
	if (lfLfPos == std::string::npos || _parseHeaderFields(_output.substr(0, lfLfPos + 1))) {
		_hasSucceeded = false;
		return ;
	}

	_body = _output.substr(lfLfPos + 2, _output.length() - (lfLfPos + 2));
	if (_fields.find("content-length") != _fields.end()) {
		if (_fields["content-length"].size() != 1 
				|| Request::parseContentLength(_fields["content-length"][0])) {
			_hasSucceeded = false;
			return ;
		}
		_body = _body.substr(0, Request::stoi(_fields["content-length"][0]));
	}

	if (_parseHeaderFieldsPostBodyParsing(_fields))
		_hasSucceeded = false;
}

int	CGI::_parseHeaderFieldsPostBodyParsing(std::map< std::string, std::vector<std::string> >& fields)
{
	if (fields.find("content-type") != fields.end() && _body == "")
		return 1;
	if (fields.find("status") != fields.end())
		if ((_body.empty() && _statusCode == "200") || (!_body.empty() && _statusCode != "200"))
			return 1;
	return 0;
}

int	CGI::_parseHeaderFields(std::string header)
{
	std::string::size_type	lfPos;
	std::string				fieldLine;

	lfPos = header.find("\n");
	while (!header.empty() && lfPos != std::string::npos) {
		fieldLine = header.substr(0, lfPos);
		if (_parseFieldLine(fieldLine))
			return 1;
		header = header.substr(lfPos + 1, header.length() - (lfPos + 1));
		lfPos = header.find("\n");
	}
	if (_fields.find("content-type") == _fields.end()
			&& _fields.find("status") == _fields.end())
		return 1;
	if ((_fields.find("content-type") != _fields.end() 
			&& (_fields["content-type"].size() != 1 || _parseContentType(_fields["content-type"][0])))
		|| (_fields.find("status") != _fields.end() && 
			(_fields["status"].size() != 1 || _parseStatus(_fields["status"][0]))))
		return 1;
	return 0;
}

int	CGI::_parseStatus(std::string status)
{
	std::string::size_type	sepPos;

	sepPos = status.find(" ");

	_statusCode = sepPos == std::string::npos ? status : status.substr(0, sepPos);
	if (_statusCode != "200" && _statusCode != "400" && _statusCode != "501")
		return 1;
	if (sepPos == std::string::npos)
		return 0;

	_reasonMessage = status.substr(sepPos + 1, status.length() - (sepPos + 1));
	for (std::string::iterator it = _reasonMessage.begin(); it != _reasonMessage.end(); it++)
		if (!Request::isVChar(*it))
			return 1;
	return 0;
}

int	CGI::_parseContentType(std::string contentType)
{
	std::string::size_type	sepPos;

	if (contentType.empty())
		return 0;
	if ((sepPos = contentType.find("/")) == std::string::npos)
		return 1;
	return !(Request::isToken(contentType.substr(0, sepPos))
		&& Request::isToken(contentType.substr(sepPos + 1, contentType.length() - (sepPos + 1))));
}

int	CGI::_parseFieldLine(std::string fieldLine)
{
	std::string				fieldName;
	std::string				fieldValue;
	std::string::size_type	delimiterPos;

	if (fieldLine.empty())
		return 1;
	delimiterPos = fieldLine.find(":");
	if (delimiterPos == std::string::npos)
		return 1;
	fieldName = fieldLine.substr(0, delimiterPos);
	if (std::isspace(fieldLine[delimiterPos + 1]))
		delimiterPos++;
	if (std::isspace(fieldLine[fieldLine.length() - 1]))
		fieldValue = fieldLine.substr(delimiterPos + 1, fieldLine.length() - (delimiterPos + 1) - 1);
	else
		fieldValue = fieldLine.substr(delimiterPos + 1, fieldLine.length() - (delimiterPos + 1));
	if (Request::parseFieldName(fieldName) || Request::parseFieldValue(fieldValue))
		return 1;
	_fields[Request::toLower(fieldName)].push_back(fieldValue);
	return 0;
}

void	CGI::_launch(void)
{
	if (pipe2(_pipeFdOut, O_CLOEXEC) == -1)
		throw std::runtime_error("The server encountered an error");

	if (_env["REQUEST_METHOD"] == "POST")
		if (pipe2(_pipeFdIn, O_CLOEXEC) == -1) {
			_closePipes("GET");
			throw std::runtime_error("The server encountered an error");
		}

	if ((_cpid = fork()) == -1) {
		_env["REQUEST_METHOD"] == "POST" ? _closePipes("") : _closePipes("GET");
		throw std::runtime_error("The server encountered an error");
	}
	
	if (_cpid == 0) {
		if (_env["REQUEST_METHOD"] == "POST")
			if (dup2(_pipeFdIn[READ_END], STDIN_FILENO) == -1) {
				_closePipes("");
				throw std::runtime_error("The server encountered an error");
			}
		if (dup2(_pipeFdOut[WRITE_END], STDOUT_FILENO) == -1) {
			_env["REQUEST_METHOD"] == "POST" ? _closePipes("") : _closePipes("GET");
			throw std::runtime_error("The server encountered an error");
		}
		if (chdir((char *)findDirectory(_cgi).c_str()) == -1)
			throw std::runtime_error("The server encountered an error");
		execve(_program.c_str(), (char*[]){(char *)_program.c_str(), (char *)_findName(_cgi).c_str(), NULL}, _envp);
		throw std::runtime_error("The server encountered an error");
	}

	_epoll->addFd(_pipeFdOut[READ_END], EPOLLIN, TIMEOUT);
	close(_pipeFdOut[WRITE_END]);
	_pipeFdOut[WRITE_END] = CLOSED;

	if (_env["REQUEST_METHOD"] == "POST") {
		_epoll->addFd(_pipeFdIn[WRITE_END], EPOLLOUT);
		close(_pipeFdIn[READ_END]);
		_pipeFdIn[READ_END] = CLOSED;
	}
}

void	CGI::_closePipes(std::string method)
{
	if (method.empty() || method == "GET") {
		close(_pipeFdOut[READ_END]);
		_pipeFdOut[READ_END] = CLOSED;
		close(_pipeFdOut[WRITE_END]);
		_pipeFdOut[WRITE_END] = CLOSED;
	}
	if (method.empty() || method == "POST") {
		close(_pipeFdIn[READ_END]);
		_pipeFdIn[READ_END] = CLOSED;
		close(_pipeFdIn[WRITE_END]);
		_pipeFdIn[WRITE_END] = CLOSED;
	}
}

void	CGI::_setEnv(void)
{
	if (_request->getBodyLength())
		_env["CONTENT_LENGTH"] = _request->getBodyLength();
	
	if (_request->getFields().find("content-type") != _request->getFields().end())
		_env["CONTENT_TYPE"] = _request->getFields()["content-type"][0];

	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	
	_env["QUERY_STRING"] = _request->getQuery();
	
	_env["REMOTE_ADDR"] = std::string(inet_ntoa(_client->getSocket().getAddr().sin_addr));
	
	_env["REQUEST_METHOD"] = _request->getMethod() == Request::GET ? "GET" : "POST";

	_env["SCRIPT_NAME"] = _getFullPath(_cgi);

	_env["SERVER_NAME"] = _request->getConf()->getServerName();
	
	_env["SERVER_PORT"] = Response::itos(_request->getServer()->getPort());
	
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	
	_env["SERVER_SOFTWARE"] = "webserv";

	if (_request->getFields().find("cookie") != _request->getFields().end())
		_env["HTTP_COOKIE"] = _createCookieString(_request->getFields()["cookie"]);
}

std::string	CGI::_createCookieString(std::vector<std::string> cookies)
{
	std::string	cookiesString;

	for (size_t i = 0; i < cookies.size() - 1; i++)
		cookiesString+=cookies[i]+";";
	cookiesString+=cookies[cookies.size() - 1];
	return cookiesString;
}

void	CGI::_convertEnv(void)
{
	_envp = new char*[_env.size() + 1];
	if (!_envp)
		throw std::runtime_error("The server encountered an error");

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
		throw std::runtime_error("The server encountered an error");
	memcpy(*varp, var.c_str(), var.length());
	(*varp)[var.length()] = 0;
}

std::string	CGI::_getFullPath(std::string path)
{
	char*	cwd;

	if (path.substr(0, 1) == "/")
		return path;
	cwd = getcwd(NULL, 0);
	if (!cwd)
		throw std::runtime_error("The server encountered an error");
	if (path.substr(0, 2) == "./")
		path = cwd+path.substr(1, path.length() - 1);
	else
		path = cwd+std::string("/")+path;
	free(cwd);
	return path;
}

std::string	CGI::findDirectory(std::string path)
{
	std::size_t	sepPos;

	sepPos = path.rfind("/");
	if (sepPos == std::string::npos)
		return "./";
	return path.substr(0, sepPos + 1);
}
std::string	CGI::_findName(std::string path)
{
	std::size_t	sepPos;

	sepPos = path.rfind("/");
	if (sepPos == std::string::npos)
		return path;
	return path.substr(sepPos + 1, path.length() - (sepPos + 1));
}
