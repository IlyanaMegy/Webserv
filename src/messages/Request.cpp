#include "Request.hpp"

#include "Server.hpp"
#include "CGI.hpp"
#include "Epoll.hpp"
#include "ServerConf.hpp"

Request::Request(void)
	: _stage(SEEKING_STATUS_LINE), _state(TREATING_MESSAGE), _response(NULL), _server(NULL), _conf(NULL), _client(NULL), _epoll(NULL), _cgi(NULL), _untreatedMessage("") , _fields(std::map< std::string, std::vector<std::string> >()), _isBodyChunked(false), _bodyLength(0), _body("") {}

Request::Request(Server* server, Client* client, Epoll* epoll, std::string leftoverMessage)
	: _stage(SEEKING_STATUS_LINE), _state(TREATING_MESSAGE), _response(server->getDefaultConf()), _server(server), _conf(server->getDefaultConf()), _client(client), _epoll(epoll), _cgi(NULL), _untreatedMessage(leftoverMessage), _fields(std::map< std::string, std::vector<std::string> >()), _isBodyChunked(false), _bodyLength(0), _body("") {}

Request::~Request(void)
{
	if (_cgi)
		delete _cgi;
}

Response	&Request::getResponse(void)
{
	return _response;
}

std::string	Request::getUntreatedMessage(void)
{
	return _untreatedMessage;
}

Request::Stage	Request::getStage(void)
{
	return _stage;
}

Request::State	Request::getState(void)
{
	return _state;
}

CGI*	Request::getCGI(void)
{
	return _cgi;
}

Server*	Request::getServer(void)
{
	return _server;
}

Client*	Request::getClient(void)
{
	return _client;
}

ServerConf*	Request::getConf(void)
{
	return _conf;
}

Request::Method	Request::getMethod(void)
{
	return _method;
}

std::string	Request::getPath(void)
{
	return _path;
}

std::string Request::getQuery(void)
{
	return _query;
}

std::map< std::string, std::vector<std::string> >&	Request::getFields(void)
{
	return _fields;
}

unsigned int	Request::getBodyLength(void)
{
	return _bodyLength;
}

std::string	Request::getBody(void)
{
	return _body;
}

void	Request::add(std::string buffer)
{
	_untreatedMessage = _untreatedMessage+buffer;
	_state = TREATING_MESSAGE;
}

void	Request::parse(void)
{
	if (_stage == SEEKING_STATUS_LINE)
		_parseStartLine();
	if (_stage == SEEKING_HEADER)
		_parseHeader();
	if (_stage == SEEKING_BODY)
		_parseBody();
}

void	Request::_treatReg(void)
{
	if (!_conf->isValidMethod(_path, _method)) {
		_response.fillError("405", "Method Not Allowed");
		_stage = DONE;
		return ;
	}

	if (_method == GET)
		_response.fillGET(_conf->getCompletePath(_path));
	else if (_method == DELETE)
		_response.fillDELETE(_conf->getCompletePath(_path));
	else
		_response.fillPOST(_conf->getCompletePath(_path), _body);
	_stage = DONE;
}

void	Request::_treatDir(void)
{
	std::string	defaultFile = _conf->getDefaultFile(_path);
	struct stat	sb;
	int			res = stat(defaultFile.c_str(), &sb);

	if (_method != GET || (!res && !S_ISREG(sb.st_mode)))
		_response.fillError("403", "Forbidden");
	else if (res) {
		if (!_conf->isAutoindexOn(_path))
			_response.fillError("403", "Forbidden");
		else
			_response.fillAutoindex(_conf->getCompletePath(_path));
	}
	else {
		_path = "/"+_conf->getIndex(_path);
		_response.setVirtualPath(_path);
		return(_conf->isCgi(_path) ? treatCGI() : _treatReg());
	}
	_stage = DONE;
}

void	Request::_treatPOST(std::string physicalPath, struct stat sb, int res)
{
	struct stat	sbDir;
	int			resDir = stat(CGI::findDirectory(physicalPath).c_str(), &sbDir);

	if (resDir) {
		_response.fillError("404", "Not Found");
		_stage = DONE;
		return ;
	}
	if (!S_ISDIR(sbDir.st_mode) || (!res && !S_ISREG(sb.st_mode))) {
		_response.fillError("403", "Forbidden");
		_stage = DONE;
		return ;
	}
	return (_treatReg());
}

void	Request::treat(void)
{
	std::string	physicalPath = _conf->getCompletePath(_path);
	struct stat	sb;
	int			res = stat(physicalPath.c_str(), &sb);

	if (_conf->isRedir(_path)) {
		_response.fillRedir(_conf->getRedirStatusCode(_path), _conf->getRedirHostname(_path));
		_stage = DONE;
		return ;
	}

	if (_method == POST && !_conf->isCgi(_path))
		return (_treatPOST(physicalPath, sb, res));
	
	if (res) {
		_response.fillError("404", "Not Found");
		_stage = DONE;
		return ;
	}

	if (S_ISREG(sb.st_mode))
		return(_conf->isCgi(_path) ? treatCGI() : _treatReg());
	if (S_ISDIR(sb.st_mode))
		return (_treatDir());

	_response.fillError("403", "Forbidden");
	_stage = DONE;
}

void	Request::treatCGI(void)
{
	if (!_cgi) {
		if (!_conf->isValidMethod(_path, _method)) {
			_response.fillError("405", "Method Not Allowed");
			_stage = DONE;
			return ;
		}
		if (_launchCGI(_conf->getCompletePath(_path))) {
			_response.fillError("502", "Bad Gateway");
			_stage = DONE;
		}
		return ;
	}
	if (!_cgi->getHasSucceeded()) {
		_response.fillError("502", "Bad Gateway");
		_stage = DONE;
		return;
	}
	_response.fillCGI(_cgi);
	_stage = DONE;
}

int	Request::_launchCGI(std::string physicalPath)
{
	std::string	executablePath;

	executablePath = _conf->getCgiExecutable(_path);
	if (access(executablePath.c_str(), X_OK) || access(physicalPath.c_str(), R_OK))
		return 1;
	_cgi = new CGI(_epoll, this, executablePath, physicalPath);
	return 0;
}

void	Request::_parseBody(void)
{
	if (_isBodyChunked)
		while (_stage != DONE && _stage != PROCESSING && _state != AWAITING_MESSAGE)
			_parseChunkedBody();
	else
		_parseFullBody();
}

void	Request::_parseFullBody(void)
{
	if (_bodyLength == 0) {
		_stage = PROCESSING;
		return ;
	}
	if (_untreatedMessage.length() < _bodyLength) {
		_state = AWAITING_MESSAGE;
		return ;
	}
	_body = _untreatedMessage.substr(0, _bodyLength);
	_untreatedMessage = _untreatedMessage.substr(_bodyLength, _untreatedMessage.length() - _bodyLength);
	_stage = PROCESSING;
}

int	Request::_parseChunkedBody(void)
{
	static unsigned int		chunkLength;
	std::string				chunk;

	if (chunkLength == 0) {
		chunkLength = _findChunkLength();
		if (_stage == DONE || _state == AWAITING_MESSAGE)
			return 1;
		if (chunkLength == 0) {
			_stage = PROCESSING;
			return 0;
		}
		_bodyLength+=chunkLength;
	}
	else {
		chunk = _findChunk(chunkLength);
		if (_stage == DONE || _state == AWAITING_MESSAGE)
			return 1;
		_body+=chunk;
		chunkLength = 0;
	}
	return 0;
}

std::string	Request::_findChunk(unsigned int chunkLength)
{
	std::string	chunk;

	if (_untreatedMessage.length() < chunkLength + 2) {
		_state = AWAITING_MESSAGE;
		return "";
	}
	if (_untreatedMessage.substr(chunkLength, 2) != "\r\n") {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return "";
	}
	chunk = _untreatedMessage.substr(0, chunkLength);
	_untreatedMessage = _untreatedMessage.substr(chunkLength + 2, _untreatedMessage.length() - (chunkLength + 2));
	return chunk;
}

unsigned int	Request::_findChunkLength(void)
{
	std::string::size_type	crlfPos;
	std::string				line;
	unsigned int			chunkLength;

	crlfPos = _untreatedMessage.find("\r\n");
	if (crlfPos == std::string::npos) {
		_state = AWAITING_MESSAGE;
		return 0;
	}
	line = _untreatedMessage.substr(0, crlfPos);
	if (line.empty()) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 0;
	}

	for (std::string::iterator it = line.begin(); it != line.end(); it++) {
		if (!_isHex(*it)) {
			_response.fillError("400", "Bad Request");
			_stage = DONE;
			return 0;
		}
	}
	chunkLength = _stoh(line);
	if (chunkLength + _bodyLength > _conf->getMaxBodySize()) {
		_response.fillError("413", "Content Too Large");
		_stage = DONE;
		return 0;
	}
	_untreatedMessage = _untreatedMessage.substr(crlfPos + 2, _untreatedMessage.length() - (crlfPos + 2));
	return chunkLength;
}

void	Request::_parseHeader(void)
{
	std::string	header;

	header = _findHeader();
	if (_stage == DONE || _state == AWAITING_MESSAGE)
		return ;
	if (_parseHeaderFields(header))
		return ;
	_stage = SEEKING_BODY;
}

std::string	Request::_findHeader(void)
{
	std::string::size_type	crlfCrlfPos;
	std::string				header;

	if (_untreatedMessage.length() > MAXHEADEROCTETS
		|| (_untreatedMessage.length() > 1 && _untreatedMessage.substr(0, 2) == "\r\n")) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return "";
	}
	crlfCrlfPos = _untreatedMessage.find("\r\n\r\n");
	if (crlfCrlfPos == std::string::npos) {
		_state = AWAITING_MESSAGE;
		return "";
	}
	header = _untreatedMessage.substr(0, crlfCrlfPos + 2);
	if (header.length() > MAXHEADEROCTETS) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return "";
	}
	_untreatedMessage = _untreatedMessage.substr(crlfCrlfPos + 4, _untreatedMessage.length() - (crlfCrlfPos + 4));
	return header;
}

int	Request::_parseHeaderFields(std::string header)
{
	std::string::size_type	crlfPos;
	std::string				fieldLine;

	crlfPos = header.find("\r\n");
	while (!header.empty() && crlfPos != std::string::npos) {
		fieldLine = header.substr(0, crlfPos);
		if (_parseFieldLine(fieldLine))
			return 1;
		header = header.substr(crlfPos + 2, header.length() - (crlfPos + 2));
		crlfPos = header.find("\r\n");
	}
	if (_parseCompletedFields())
		return 1;
	return 0;
}

int	Request::_parseCompletedFields(void)
{
	if (_findHost())
		return 1;
	if (_findTransferEncoding())
		return 1;
	if (_findContentLength())
		return 1;
	_findConnection();
	_findCookies();
	return 0;
}

void	Request::_findCookies(void)
{
	if (_fields.find("cookie") == _fields.end())
		return ;
	_split(_fields["cookie"]);
	_response.addCookies(_fields["cookie"]);
}

void	Request::_findConnection(void)
{
	if (_fields.find("connection") == _fields.end())
		return ;
	_split(_fields["connection"]);
	for (std::vector<std::string>::iterator it = _fields["connection"].begin(); it != _fields["connection"].end(); it++) {
		if (*it == "close")
			_response.setShouldClose(true);
	}
}

int	Request::_findHost(void)
{
	if (_fields.find("host") == _fields.end()
			|| _fields["host"].size() != 1
			|| _parseAuthority(_fields["host"][0])) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 1;
	}
	_conf = _server->getConfig(_fields["host"][0]);
	_response.setConf(_conf);
	return 0;
}

int	Request::_findTransferEncoding(void)
{
	if (_fields.find("transfer-encoding") == _fields.end())
		return 0;
	if (_fields["transfer-encoding"].size() != 1
		|| toLower(_fields["transfer-encoding"][0]) != "chunked") {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 1;
	}
	_isBodyChunked = true;
	return 0;
}

int	Request::_findContentLength(void)
{
	if (_fields.find("content-length") == _fields.end()) {
		if (_method == GET || _method == DELETE || _isBodyChunked) {
			_bodyLength = 0;
			return 0;
		}
		_response.fillError("411", "Length Required");
		_stage = DONE;
		return 1;
	}
	if (_fields["content-length"].size() != 1
			|| parseContentLength(_fields["content-length"][0])) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 1;
	}
	if (_isBodyChunked)
		return 0;
	_bodyLength = stoi(_fields["content-length"][0]);
	if (_bodyLength > _conf->getMaxBodySize()) {
		_response.fillError("413", "Content Too Large");
		_stage = DONE;
		return 1;
	}
	return 0;
}

int	Request::parseContentLength(std::string contentLength)
{
	if (contentLength.empty())
		return 1;
	for (std::string::iterator it = contentLength.begin(); it != contentLength.end(); it++) {
		if (!std::isdigit(*it))
			return 1;
	}
	return 0;
}

int	Request::_parseFieldLine(std::string fieldLine)
{
	std::string				fieldName;
	std::string				fieldValue;
	std::string::size_type	delimiterPos;

	if (fieldLine.empty()) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 1;
	}
	delimiterPos = fieldLine.find(":");
	if (delimiterPos == std::string::npos) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 1;
	}

	fieldName = fieldLine.substr(0, delimiterPos);
	if (std::isspace(fieldLine[delimiterPos + 1]))
		delimiterPos++;
	if (std::isspace(fieldLine[fieldLine.length() - 1]))
		fieldValue = fieldLine.substr(delimiterPos + 1, fieldLine.length() - (delimiterPos + 1) - 1);
	else
		fieldValue = fieldLine.substr(delimiterPos + 1, fieldLine.length() - (delimiterPos + 1));
	if (parseFieldName(fieldName) || parseFieldValue(fieldValue)) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 1;
	}
	_fields[toLower(fieldName)].push_back(fieldValue);
	return 0;
}

int	Request::parseFieldName(std::string fieldName)
{
	return (!isToken(fieldName));
}

int	Request::parseFieldValue(std::string fieldValue)
{
	if (fieldValue.empty())
		return 0;
	if (!isVChar(fieldValue[0]) && !_isObsText(fieldValue[0]))
		return 1;
	if (fieldValue.length() == 1)
		return 0;
	for (size_t i = 1; i < fieldValue.length() - 1; i++) {
		if (!isVChar(fieldValue[i]) && !_isObsText(fieldValue[i])
			&& fieldValue[i] != ' ' && fieldValue[i] != '\t')
			return 1;
	}
	if (!isVChar(fieldValue[fieldValue.length() - 1]) && !_isObsText(fieldValue[fieldValue.length() - 1]))
		return 1;
	return 0;
}

void	Request::_parseStartLine(void)
{
	std::string	startLine;

	startLine = _findStartLine();
	if (_stage == DONE || _state == AWAITING_MESSAGE)
		return ;
	if (_parseRequestLine(startLine))
		return ;
	_stage = SEEKING_HEADER;
}

std::string	Request::_findStartLine(void)
{
	std::string::size_type	crlfPos;
	std::string				startLine;

	if (_untreatedMessage.length() > MAXSTATUSLINEOCTETS) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return "";
	}
	crlfPos = _untreatedMessage.find("\r\n");
	if (crlfPos == std::string::npos) {
		_state = AWAITING_MESSAGE;
		return "";
	}
	startLine = _untreatedMessage.substr(0, crlfPos);
	if (startLine.length() > MAXSTATUSLINEOCTETS) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return "";
	}
	_untreatedMessage = _untreatedMessage.substr(crlfPos + 2, _untreatedMessage.length() - (crlfPos + 2));
	return startLine;
}

int	Request::_parseRequestLine(std::string startLine)
{
	std::string::size_type	sp1Pos;
	std::string::size_type	sp2Pos;

	sp1Pos = startLine.find(" ");
	sp2Pos = startLine.find(" ", sp1Pos + 1);
	if (sp1Pos == std::string::npos || sp2Pos == std::string::npos
		|| startLine.find(" ", sp2Pos + 1) != std::string::npos) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 1;
	}

	if (_parseMethod(startLine.substr(0, sp1Pos)))
		return 1;
	if (_parseUri(startLine.substr(sp1Pos + 1, sp2Pos - (sp1Pos + 1))))
		return 1;
	if (_parseHTTPVer(startLine.substr(sp2Pos + 1, startLine.length() - (sp2Pos + 1))))
		return 1;
	return 0;
}

int	Request::_parseMethod(std::string methodString)
{
	if (methodString == "GET")
		_method = GET;
	else if (methodString == "POST")
		_method = POST;
	else if (methodString == "DELETE")
		_method = DELETE;
	else {
		_response.fillError("501", "Not Implemented");
		_stage = DONE;
		return 1;
	}
	return 0;
}

int	Request::_parseUri(std::string uri)
{
	std::string::size_type	schemeEndPos;
	std::string::size_type	pathStartPos;
	std::string::size_type	queryStartPos;

	schemeEndPos = uri.find("://");
	if (schemeEndPos != std::string::npos) {
		if (_parseScheme(uri.substr(0, schemeEndPos))) {
			_response.fillError("400", "Bad Request");
			_stage = DONE;
			return 1;
		}
	}
	pathStartPos = uri.find('/', schemeEndPos == std::string::npos ? 0 : schemeEndPos + 3);
	if (pathStartPos == std::string::npos || (schemeEndPos != std::string::npos && pathStartPos == schemeEndPos + 3)
			|| _parseAuthority(schemeEndPos == std::string::npos ? uri.substr(0, pathStartPos) : uri.substr(schemeEndPos + 3, pathStartPos - (schemeEndPos + 3)))) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 1;
	}
	queryStartPos = uri.find('?', pathStartPos + 1);
	_path = uri.substr(pathStartPos, queryStartPos == std::string::npos ? uri.length() - pathStartPos : queryStartPos - pathStartPos);
	_response.setVirtualPath(_path);
	if (queryStartPos != std::string::npos && _parseQuery(uri.substr(queryStartPos + 1, uri.length() - (queryStartPos + 1)))) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 1;
	}
	return 0;
}

int	Request::_parseQuery(std::string query)
{
	std::string::size_type	sepPos;
	std::string::size_type	affectPos;
	std::string::size_type	pairStart = 0;
	std::string				pair;

	if (query.empty())
		return 0;
	sepPos = query.find('&');
	while (sepPos != std::string::npos) {
		pair = query.substr(pairStart, sepPos - pairStart);
		affectPos = pair.find('=');
		if (affectPos == 0)
			return 1;
		_arguments[pair.substr(0, affectPos == std::string::npos ? pair.length() : affectPos)]
			= affectPos == std::string::npos ? "" : pair.substr(affectPos + 1, pair.length() - (affectPos + 1));
		pairStart = sepPos + 1;
		sepPos = query.find('&', pairStart);
	}
	pair = query.substr(pairStart, query.length() - pairStart);
	affectPos = pair.find('=');
	if (affectPos == 0)
		return 1;
	_arguments[pair.substr(0, affectPos == std::string::npos ? pair.length() : affectPos)]
		= affectPos == std::string::npos ? "" : pair.substr(affectPos + 1, pair.length() - (affectPos + 1));
	_query = query;
	return 0;
}

int	Request::_parseAuthority(std::string authority)
{
	std::string::size_type	portStartPos;
	std::string				host;

	if (authority.empty())
		return 0;
	portStartPos = authority.find(':');
	if (portStartPos == 0)
		return 1;
	if (portStartPos != std::string::npos
			&& stoi(authority.substr(portStartPos + 1, authority.length() - (portStartPos + 1))) != _server->getPort())
		return 1;
	host = authority.substr(0, portStartPos == std::string::npos ? authority.length() : portStartPos);
	if (!_host.empty() && host != _host)
		return 1;
	_host = host;
	return 0;
}

int	Request::_parseScheme(std::string scheme)
{
	return scheme != "http";
}

int	Request::_parseHTTPVer(std::string HTTPVerString)
{
	if (HTTPVerString.length() != 8 || HTTPVerString.substr(0, 5) != "HTTP/"
		|| HTTPVerString[6] != '.' || !std::isdigit(HTTPVerString[5]) || !std::isdigit(HTTPVerString[7])) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 1;
	}
	if (HTTPVerString[5] != '1') {
		_response.fillError("505", "HTTP Version Not Supported");
		_stage = DONE;
		return 1;
	}
	return 0;
}

std::string	Request::toLower(std::string s)
{
	std::string	lowerS;

	if (s.empty())
		return "";
	for (std::string::iterator it = s.begin(); it != s.end(); it++)
		lowerS.push_back(std::tolower(*it));
	return lowerS;
}

bool	Request::isDelimiter(unsigned char c)
{
	static	std::string	delimiters("\"(),/:;<=>?@[\\]{}");

	return (delimiters.find(c) != std::string::npos);
}

bool	Request::isVChar(unsigned char c)
{
	return (32 < c && c < 127);
}

bool	Request::_isObsText(unsigned char c)
{
	return (127 < c);
}

bool	Request::_isHex(unsigned char c)
{
	return (std::isdigit(c) || c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F');
}

bool	Request::isToken(std::string s)
{
	if (s.empty())
		return false;
	for (std::string::iterator it = s.begin(); it != s.end(); it++) {
		if (!isVChar(*it) || isDelimiter(*it))
			return false;
	}
	return true;
}

unsigned int	Request::stoi(std::string value)
{
	unsigned int		res;
	std::istringstream	stream(value);

	stream >> res;
	return res;
}

unsigned int	Request::_stoh(std::string value)
{
	unsigned int		res;
	std::stringstream	stream;

	stream << std::hex << value;
	stream >> res;
	return res;
}

void	Request::_split(std::vector<std::string>& vector)
{
	std::vector<std::string>	copy = vector;

	vector = std::vector<std::string>();
	for (std::vector<std::string>::iterator it = copy.begin(); it!= copy.end(); it++) {
		std::stringstream	value(*it);
		std::string			splitValue;

		while (std::getline(value, splitValue, ',')) {
			splitValue.erase(remove_if(splitValue.begin(), splitValue.end(), isspace), splitValue.end());
			vector.push_back(splitValue);
		}
	}
}
