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

void	Request::parse(std::string buffer)
{

}

void	Request::_parseStartLine(void)
{

}

