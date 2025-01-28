#include "Response.hpp"

Response::Response(void) : _message(""), _statusCode("") {}

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

}

void	Response::_createBody(void)
{

}

