#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include <vector>
# include <cctype>
# include <iostream>

# include "Response.hpp"

# define MAXSTATUSLINEOCTETS 2000
# define MAXHEADEROCTETS 4000
# define MAXBODYOCTETS 2000000000

class Request
{

	public:
		enum Method {
			GET,
			POST,
			DELETE
		};

		Request(void);
		Request(std::string leftoverMessage);
		~Request(void);

		Response					&getResponse(void);
		std::string					getUntreatedMessage(void);

		void						parse(std::string buffer);

	private:

		enum STAGE {
			SEEKING_STATUS_LINE,
			SEEKING_HEADER,
			SEEKING_BODY,
			PROCESSING,
			DONE
		};

		enum STATE {
			AWAITING_MESSAGE,
			TREATING_MESSAGE
		};

		STAGE												_stage;
		STATE												_state;

		Response											_response;

		std::string											_untreatedMessage;
	
		Method												_method;
		std::string											_uri;
		std::map< std::string, std::vector<std::string> >	_fields;
		bool												_isBodyChunked;
		unsigned int										_bodyLength;
		std::string											_body;

		void						_parseStartLine(void);
		std::string					_findStartLine(void);
		int							_parseRequestLine(std::string startLine);
		int							_parseMethod(std::string startLine, std::string::size_type sp1Pos);
		int							_parseUri(std::string startLine, std::string::size_type sp1Pos, std::string::size_type sp2Pos);
		int							_parseHTTPVer(std::string startLine, std::string::size_type sp2Pos);

		void						_parseHeader(void);
		std::string					_findHeader(void);
		int							_parseHeaderFields(std::string header);
		int							_parseFieldLine(std::string fieldLine);
		int							_parseFieldName(std::string fieldName);
		int							_parseFieldValue(std::string fieldValue);
		int							_parseCompletedFields(void);
		int							_findTransferEncoding(void);
		int							_findContentLength(void);
		int							_parseContentLength(std::string contentLength);

		void						_parseBody(void);

		void						_treat(void);

		static std::string			_toLower(std::string s);
		static bool					_isDelimiter(unsigned char c);
		static bool					_isVChar(unsigned char c);
		static bool					_isObsText(unsigned char c);
		static unsigned int			_stoi(std::string value);

};

#endif
