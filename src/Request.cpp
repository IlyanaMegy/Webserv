#include "Request.hpp"

Request::Request(void)
	: _isComplete(false), _method(NONE), _uri(""), _fields(std::map< std::string, std::vector<std::string> >())  {}

Request::~Request(void) {}

bool	Request::getIsComplete(void) const
{
	return _isComplete;
}

Request::Method	Request::getMethod(void) const
{
	return _method;
}

std::string	Request::getUri(void) const
{
	return _uri;
}

std::vector<std::string>	Request::getFieldValues(std::string fieldName)
{
	return _fields[fieldName];
}

Response	&Request::getResponse(void)
{
	return _response;
}

void	Request::parse(std::string buffer)
{
	_untreatedMessage = _untreatedMessage+buffer;
	if (_method == NONE)
		_parseStartLine();
}

void	Request::_parseStartLine(void)
{
	std::string	startLine;

	startLine = _findStartLine();
	if (startLine.empty())
		return ;
	_parseRequestLine(startLine);
}

std::string	Request::_findStartLine(void)
{
	std::string::size_type	crlfPos;
	std::string				startLine;

	if (_untreatedMessage.length() > MAXOCTETS) {
		_fillResponse("400", "Bad Request", true);
		return "";
	}
	crlfPos = _untreatedMessage.find("\r\n");
	if (crlfPos == std::string::npos)
		return "";
	startLine = _untreatedMessage.substr(0, crlfPos);
	if (startLine.length() > MAXOCTETS) {
		_fillResponse("400", "Bad Request", true);
		return "";
	}
	_untreatedMessage = _untreatedMessage.substr(crlfPos + 2, _untreatedMessage.length() - (crlfPos + 2));
	return startLine;
}

void	Request::_parseRequestLine(std::string startLine)
{
	std::string::size_type	sp1Pos;
	std::string::size_type	sp2Pos;

	sp1Pos = startLine.find(" ");
	sp2Pos = startLine.find(" ", sp1Pos + 1);
	if (sp1Pos == std::string::npos || sp2Pos == std::string::npos
		|| startLine.find(" ", sp2Pos + 1) != std::string::npos) {
		_fillResponse("400", "Bad Request", true);
		return ;
	}

	if (_parseMethod(startLine, sp1Pos))
		return ;
	if (_parseUri(startLine, sp1Pos, sp2Pos))
		return ;
	_parseHTTPVer(startLine, sp2Pos);
}

void	Request::_fillResponse(std::string statusCode, std::string reasonMessage, bool isComplete)
{
	_response.setStatusCode(statusCode);
	_response.setReasonMessage(reasonMessage);
	_isComplete = isComplete;
}

