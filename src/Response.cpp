#include "Response.hpp"

Response::Response(void) : _statusCode(""), _message("") {}

Response::~Response(void) {}

void	Response::setStatusCode(std::string statusCode)
{
	_statusCode = statusCode;
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
	
}
