#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include <vector>

# include "Response.hpp"

# define MAXOCTETS 4000

class Request
{

	public:
		enum Method {
			GET,
			POST,
			DELETE
		};

		Request(void);
		~Request(void);

		Method						getMethod(void) const;
		std::string					getUri(void) const;
		std::vector<std::string>	getFieldValues(std::string fieldName);
		Response					&getResponse(void);

		void						parse(std::string buffer);

	private:

		enum STAGE {
			SEEKING_STATUS_LINE,
			SEEKING_HEADER,
			PROCESSING,
			SEEKING_BODY,
			DONE
		};

		STAGE												_stage;

		Response											_response;

		std::string											_untreatedMessage;
	
		Method												_method;
		std::string											_uri;
		std::map< std::string, std::vector<std::string> >	_fields;

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

		void						_treat(void);

		static std::string			_toLower(std::string s);
		static bool					_isDelimiter(unsigned char c);
		static bool					_isVChar(unsigned char c);
		static bool					_isObsText(unsigned char c);

};

#endif
