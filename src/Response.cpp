#include "Response.hpp"

Response::Response(void) : _message(""), _shouldClose(false), _statusCode("")
{
	_fields["Server"].push_back(SERVERNAME);
}

Response::~Response(void) {}

void	Response::setStatusCode(std::string statusCode)
{
	_statusCode = statusCode;
}

void	Response::setReasonMessage(std::string reasonMessage)
{
	_reasonMessage = reasonMessage;
}

std::string	Response::getMessage(void) const
{
	return _message;
}

std::string	Response::getStatusCode(void) const
{
	return _statusCode;
}

bool	Response::getShouldClose(void) const
{
	return _shouldClose;
}

void	Response::createMessage(void)
{
	_createStatusLine();
	_message+="\r\n";
	_createHeader();
	_message+="\r\n";
	_createBody();
}

void	Response::_createStatusLine(void)
{
	_message=std::string("HTTP/1.1")+" "+_statusCode+" "+_reasonMessage;
}

void	Response::_createHeader(void)
{
	_updateDate();
	_updateConnection();

	for (std::map< std::string, std::vector<std::string> >::iterator it = _fields.begin(); it != _fields.end(); it++) {
		for (size_t i = 0; i != it->second.size(); i++)
			_message+=it->first+": "+it->second[i]+"\r\n";
	}
}

void	Response::_createBody(void)
{

}

void	Response::_updateDate(void)
{
	char	buffer[TIMEBUFFERSIZE];

	time_t		now = time(0);
	struct tm	tm = *gmtime(&now);

	std::strftime(buffer, TIMEBUFFERSIZE - 1, "%a, %d %b %Y %H:%M:%S %Z", &tm);
	_fields["Date"].push_back(std::string(buffer));
}

void	Response::_updateConnection(void)
{
	if (_statusCode[0] == '4' || _statusCode[0] == '5') {
		_fields["Connection"].push_back("close");
		_shouldClose = true;
	}
}