#include "Request.hpp"

Request::Request(void)
	: _stage(SEEKING_STATUS_LINE), _untreatedMessage("") , _uri(""), _fields(std::map< std::string, std::vector<std::string> >()), _bodyLength(0), _body("") {}

Request::Request(std::string leftoverMessage)
	: _stage(SEEKING_STATUS_LINE), _untreatedMessage(leftoverMessage), _uri(""), _fields(std::map< std::string, std::vector<std::string> >()), _bodyLength(0), _body("") {}

Request::~Request(void) {}

Response	&Request::getResponse(void)
{
	return _response;
}

std::string	Request::getUntreatedMessage(void)
{
	return _untreatedMessage;
}

void	Request::parse(std::string buffer)
{

	_untreatedMessage = _untreatedMessage+buffer;
	if (_stage == SEEKING_STATUS_LINE)
		_parseStartLine();
	if (_stage == SEEKING_HEADER)
		_parseHeader();
	if (_stage == SEEKING_BODY)
		_parseBody();
	if (_stage == PROCESSING)
		_treat();
}

void	Request::_treat(void)
{
	if (_method == GET)
		_response.fillGET(_uri);
	else if (_method == DELETE)
		_response.fillDELETE(_uri);
	else
		_response.fillPOST(_uri);
	_stage = DONE;
}

void	Request::_parseBody(void)
{
	if (_bodyLength == 0) {
		_stage = PROCESSING;
		return ;
	}
	if (_untreatedMessage.length() < _bodyLength)
		return ;
	_body = _untreatedMessage.substr(0, _bodyLength);
	_untreatedMessage = _untreatedMessage.substr(_bodyLength, _untreatedMessage.length() - _bodyLength);
	_stage = PROCESSING;
}

void	Request::_parseHeader(void)
{
	std::string	header;

	header = _findHeader();
	if (header.empty())
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
	if (crlfCrlfPos == std::string::npos)
		return "";
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
	if (_findContentLength())
		return 1;
	return 0;
}

int	Request::_findContentLength(void)
{
	if (_fields.find(_toLower("Content-Length")) == _fields.end()) {
		if (_method == GET || _method == DELETE) {
			_bodyLength = 0;
			return 0;
		}
		_response.fillError("411", "Length Required");
		_stage = DONE;
		return 1;
	}
	if (_fields[_toLower("Content-Length")].size() != 1
			|| _parseContentLength(_fields[_toLower("Content-Length")][0])) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 1;
	}
	_bodyLength = _stoi(_fields[_toLower("Content-Length")][0]);
	if (_bodyLength > MAXBODYOCTETS) {
		_response.fillError("413", "Content Too Large");
		_stage = DONE;
		return 1;
	}
	return 0;
}

int	Request::_parseContentLength(std::string contentLength)
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
	if (_parseFieldName(fieldName) || _parseFieldValue(fieldValue)) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 1;
	}
	_fields[_toLower(fieldName)].push_back(_toLower(fieldValue));//
	return 0;
}

int	Request::_parseFieldName(std::string fieldName)
{
	for (std::string::iterator it = fieldName.begin(); it != fieldName.end(); it++) {
		if (!_isVChar(*it) || _isDelimiter(*it))
			return 1;
	}
	return 0;
}

int	Request::_parseFieldValue(std::string fieldValue)
{
	if (fieldValue.empty())
		return 0;
	if (!_isVChar(fieldValue[0]) && !_isObsText(fieldValue[0]))
		return 1;
	if (fieldValue.length() == 1)
		return 0;
	for (size_t i = 1; i < fieldValue.length() - 1; i++) {
		if (!_isVChar(fieldValue[i]) && !_isObsText(fieldValue[i])
			&& fieldValue[i] != ' ' && fieldValue[i] != '\t')
			return 1;
	}
	if (!_isVChar(fieldValue[fieldValue.length() - 1]) && !_isObsText(fieldValue[fieldValue.length() - 1]))
		return 1;
	return 0;
}

void	Request::_parseStartLine(void)
{
	std::string	startLine;

	startLine = _findStartLine();
	if (startLine.empty())
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
	if (crlfPos == std::string::npos)
		return "";
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

	if (_parseMethod(startLine, sp1Pos))
		return 1;
	if (_parseUri(startLine, sp1Pos, sp2Pos))
		return 1;
	if (_parseHTTPVer(startLine, sp2Pos))
		return 1;
	return 0;
}

int	Request::_parseMethod(std::string startLine, std::string::size_type sp1Pos)
{
	std::string	methodString;

	methodString = startLine.substr(0, sp1Pos);
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

int	Request::_parseUri(std::string startLine, std::string::size_type sp1Pos, std::string::size_type sp2Pos)
{
	_uri = startLine.substr(sp1Pos + 1, sp2Pos - (sp1Pos + 1));
	return 0;
}

int	Request::_parseHTTPVer(std::string startLine, std::string::size_type sp2Pos)
{
	std::string	HTTPVerString;

	HTTPVerString = startLine.substr(sp2Pos + 1, startLine.length() - (sp2Pos + 1));
	if (HTTPVerString.length() != 8 || HTTPVerString.substr(0, 5) != "HTTP/"
		|| HTTPVerString[6] != '.' || !std::isdigit(HTTPVerString[5]) || !std::isdigit(HTTPVerString[7])) {
		_response.fillError("400", "Bad Request");
		_stage = DONE;
		return 1;
	}
	if (HTTPVerString[5] != '1') {
		_response.fillError("505", "Not Implemented");
		_stage = DONE;
		return 1;
	}
	return 0;
}

std::string	Request::_toLower(std::string s)
{
	std::string	lowerS;

	if (s.empty())
		return "";
	for (std::string::iterator it = s.begin(); it != s.end(); it++)
		lowerS.push_back(std::tolower(*it));
	return lowerS;
}

bool	Request::_isDelimiter(unsigned char c)
{
	static	std::string	delimiters("\"(),/:;<=>?@[\\]{}");

	return (delimiters.find(c) != std::string::npos);
}

bool	Request::_isVChar(unsigned char c)
{
	return (32 < c && c < 127);
}

bool	Request::_isObsText(unsigned char c)
{
	return (127 < c);
}

unsigned int	Request::_stoi(std::string value)
{
	unsigned int		res;
	std::istringstream	stream(value);

	stream >> res;
	return res;
}

