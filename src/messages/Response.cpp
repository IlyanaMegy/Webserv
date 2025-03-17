#include "../../inc/messages/Response.hpp"
#include "../../inc/messages/ServerConf.hpp"

Response::Response(void) : _message(""), _shouldClose(false), _isComplete(false), _statusCode(""), _reasonMessage(""), _content(""), _path("") {}
Response::~Response(void)
{}

void	Response::setStatusCode(std::string statusCode)
{
	_statusCode = statusCode;
}

void	Response::setReasonMessage(std::string reasonMessage)
{
	_reasonMessage = reasonMessage;
}

void	Response::setShouldClose(bool shouldClose)
{
	_shouldClose = shouldClose;
}

void	Response::setServerConf(ServerConf* serverConf)
{
	_defaultConf = serverConf;
}

std::string	Response::getMessage(void) const
{
	return _message;
}

bool	Response::getShouldClose(void) const
{
	return _shouldClose;
}

bool	Response::getIsComplete(void) const
{
	return _isComplete;
}

void	Response::createMessage(void)
{
	_createStatusLine();
	_message+="\r\n";
	_createHeader();
	_message+="\r\n";
	_createBody();
}

void	Response::addCookies(std::vector<std::string>& cookies)
{
	_fields["Set-Cookie"] = cookies;
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
	_message+=_content;
}

void	Response::fillError(std::string statusCode, std::string reasonMessage)
{
	_fillStatusLine(statusCode, reasonMessage);
	_shouldClose = true;
	_fillHeader();
	_fillContent(ERRORPATH+statusCode+".html");
	_isComplete = true;
}

void	Response::fillGET(std::string path)
{
	if (_fillContent(path))
		return ;
	_fillStatusLine("200", "OK");
	_fillHeader();
	_isComplete = true;
}

void	Response::fillDELETE(std::string path)
{
	if (_deleteTarget(path))
		return ;
	_fillStatusLine("204", "No Content");
	_fillHeader();
	_isComplete = true;
}

void	Response::fillPOST(std::string path, std::string body)
{
	if (_createTarget(path, body)) {
		fillError("500", "Internal Server Error");
		return ;
	}
	_fillStatusLine("201", "Created");
	_fillHeader();
	_isComplete = true;
}

int	Response::_createTarget(std::string path, std::string body)
{
	std::ofstream	ofs(path.c_str());

	if (ofs.fail())
		return 1;
	ofs << body;
	return 0;
}

int	Response::_deleteTarget(std::string path)
{
	if (access(path.c_str(), F_OK)) {
		fillError("404", "Not found");
		return 1;
	}
	if (std::remove(path.c_str())) {
		fillError("500", "Internal Server Error");
		return 1;
	}
	return 0;
}

void	Response::_fillStatusLine(std::string statusCode, std::string reasonMessage)
{
	_statusCode = statusCode;
	_reasonMessage = reasonMessage;
}

void	Response::_fillHeader(void)
{
	_fields["Server"].push_back(SERVERNAME);
	if (_shouldClose)
		_fields["Connection"].push_back("close");
	else
		_fields["Connection"].push_back("keep-alive");
	if (!_content.empty())
		_fields["Content-Length"].push_back(_itos(_content.length()));
	_updateDate();
}

int	Response::_fillContent(std::string path)
{
	std::string		line;
	std::ifstream	ifs(_fixPath(path).c_str());

	if (ifs.fail()) {
		fillError("404", "Not found");
		return 1;
	}
	while (!ifs.eof()) {
		std::getline(ifs, line);
		_content+=line+"\n";
	}
	_content.erase(_content.size() - 1, 1);
	return 0;
}

void	Response::_updateDate(void)
{
	char	buffer[TIMEBUFFERSIZE];

	time_t		now = time(0);
	struct tm	tm = *gmtime(&now);

	std::strftime(buffer, TIMEBUFFERSIZE - 1, "%a, %d %b %Y %H:%M:%S %Z", &tm);
	_fields["Date"].push_back(std::string(buffer));
}

std::string	Response::_itos(int value)
{
	std::string			res;
	std::ostringstream	stream;

	stream << value;
	res = stream.str();
	return res;
}

std::string    Response::_fixPath(std::string path)
{
	if (path.empty())
        return path;
    if (path[0] == '/') {
		if (path.length() == 1)
			return _defaultConf->getIndexLocation(path);
		if (_defaultConf->isCgi(path))
			return _defaultConf->getCgiPathForScript(path);
		return _defaultConf->getLocationCompletePath(path) + path.substr(1, path.length() - 1);
		// return path.substr(1, path.length() - 1);
	}
    return path;
}
