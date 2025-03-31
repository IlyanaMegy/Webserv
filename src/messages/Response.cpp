#include "Response.hpp"

#include "ServerConf.hpp"

#include "CGI.hpp"

Response::Response(void) : _message(""), _shouldClose(false), _isComplete(false), _conf(NULL), _statusCode(""), _reasonMessage(""), _content("") {}

Response::Response(ServerConf* conf) : _message(""), _shouldClose(false), _isComplete(false), _conf(conf), _statusCode(""), _reasonMessage(""), _content("") {}

Response::~Response(void) {}

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

void	Response::setVirtualPath(std::string virtualPath)
{
	_virtualPath = virtualPath;
}

void	Response::setConf(ServerConf* conf)
{
	_conf = conf;
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
	_fillContent(_conf->getPathErrorPage(statusCode));
	_fillStatusLine(statusCode, reasonMessage);
	_shouldClose = true;
	_fillErrorHeader();
	_fillHeader();
	_isComplete = true;
}

void	Response::fillAutoindex(std::string path)
{
	if (_fillAutoindexPage(path)) {
		fillError("500", "Internal Server Error");
		return ;
	}
	_fillStatusLine("200", "OK");
	_fillHeader();
	_isComplete = true;
}

void	Response::fillRedir(std::string statusCode, std::string newHostname)
{
	static std::map<std::string, std::string>			redirMap = _initRedirMap();

	_fillStatusLine(statusCode, redirMap[statusCode]);
	_fields["Location"].push_back(newHostname);
	_fillHeader();
	_isComplete = true;
}

void	Response::fillCGI(CGI* cgi)
{
	std::map< std::string, std::vector<std::string> > fields = cgi->getFields();

	if (fields.find("status") == fields.end())
		_fillStatusLine("200", "OK");
	else {
		if (cgi->getStatusCode() != "200") {
			fillError(cgi->getStatusCode(), cgi->getReasonMessage());
			return ;
		}
		_fillStatusLine(cgi->getStatusCode(), cgi->getReasonMessage());
		fields.erase("status");
	}
	_content = cgi->getBody();
	_fillHeader(fields);
	_isComplete = true;
}

void	Response::fillGET(std::string path)
{
	if (_fillContent(path)) {
		fillError("404", "Not found");
		return ;
	}
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
	_fields["Location"].push_back(_virtualPath);
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

void	Response::_fillHeader(std::map< std::string, std::vector<std::string> > fields)
{
	_fields["Server"].push_back(SERVERNAME);
	if (_shouldClose)
		_fields["Connection"].push_back("close");
	else
		_fields["Connection"].push_back("keep-alive");
	if (!_content.empty())
		_fields["Content-Length"].push_back(itos(_content.length()));
	_updateDate();

	if (!fields.empty())
		for (std::map< std::string, std::vector<std::string> >::iterator it = fields.begin(); it != fields.end(); it++)
			if (_fields.find(it->first) == _fields.end())
				_fields[it->first] = it->second;
}

void	Response::_fillErrorHeader(void)
{
	if (_statusCode == "405") {
		std::string						validMethodsString;
		std::vector<Request::Method>	validMethods = _conf->getValidMethods(_virtualPath);
		for (std::vector<Request::Method>::iterator it = validMethods.begin(); it != validMethods.end(); it++)
			validMethodsString+=(*it == Request::GET ? "GET" : (*it == Request::POST ? "POST" : "DELETE"))+std::string(", ");
		validMethodsString = validMethodsString.empty() ? validMethodsString : validMethodsString.substr(0, validMethodsString.length() - 2);
		_fields["Allow"].push_back(validMethodsString);
	}
}

int	Response::_fillAutoindexPage(std::string path)
{
	std::set<std::string>		fileNames;
	std::string					file;
	DIR*						dirp;
	struct dirent*				direntp;

	if (!(dirp = opendir(path.c_str())))
		return 1;
	while ((direntp = readdir(dirp))) {
		file = direntp->d_name;
		if (direntp->d_type == DT_DIR)
			file+="/";
		fileNames.insert(file);
	}
	closedir(dirp);
	fileNames.erase("./");

	_content ="\
<html>\n\
<head><title>Index of "+path+"</title></head>\n\
<body>\n\
<h1>Index of "+path+"</h1><hr><pre>\n";
	for (std::set<std::string>::iterator it = fileNames.begin(); it != fileNames.end(); it++)
		_content+="<a href="+*it+">"+*it+"</a>\n";
	_content+="\
</pre><hr></body>\n\
</html>\n";
	return 0;
}

int	Response::_fillContent(std::string path)
{
	std::string		line;
	std::ifstream	ifs(path.c_str());

	if (ifs.fail())
		return 1;
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

std::string	Response::itos(int value)
{
	std::string			res;
	std::ostringstream	stream;

	stream << value;
	res = stream.str();
	return res;
}

std::map<std::string, std::string>	Response::_initRedirMap(void)
{
	static std::map<std::string, std::string> redirMap;

	redirMap["300"] = "Multiple Choices";
	redirMap["301"] = "Moved Permanently";
	redirMap["302"] = "Found";
	redirMap["303"] = "See Other";
	redirMap["304"] = "Not Modified";
	redirMap["307"] = "Temporary Redirect";
	redirMap["308"] = "Permanent Redirect";

	return redirMap;
}
