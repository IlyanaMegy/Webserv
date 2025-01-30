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
			NONE,
			GET,
			POST,
			DELETE
		};

		Request(void);
		~Request(void);

		bool						getIsComplete(void) const;
		Method						getMethod(void) const;
		std::string					getUri(void) const;
		std::vector<std::string>	getFieldValues(std::string fieldName);
		Response					&getResponse(void);

		void						parse(std::string buffer);

	private:

		bool												_isComplete;
		Response											_response;

		std::string											_untreatedMessage;
	
		Method												_method;
		std::string											_uri;
		std::map< std::string, std::vector<std::string> >	_fields;

		void						_parseStartLine(void);
		std::string					_findStartLine(void);
		void						_parseRequestLine(std::string startLine);
		int							_parseMethod(std::string startLine, std::string::size_type sp1Pos);
		int							_parseUri(std::string startLine, std::string::size_type sp1Pos, std::string::size_type sp2Pos);
		int							_parseHTTPVer(std::string startLine, std::string::size_type sp2Pos);

		void						_parseHeader(void);
		std::string					_findHeader(void);
		void						_parseHeaderFields(std::string header);

		void						_fillResponse(std::string statusCode, std::string reasonMessage, bool isComplete);

};

#endif
