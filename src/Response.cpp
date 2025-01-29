#include "Response.hpp"

Response::Response(void) : _message(""), _statusCode("")
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

	for (std::map< std::string, std::vector<std::string> >::iterator it = _fields.begin(); it != _fields.end(); it++) {
		for (size_t i = 0; i != it->second.size(); i++)
			_message+=it->first+": "+it->second[i]+"\r\n";
	}
}

void	Response::_createBody(void)
{

}

