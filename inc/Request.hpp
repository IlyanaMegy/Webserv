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

class	Server;

class Request
{

	public:
		enum Method {
			GET,
			POST,
			DELETE
		};

		enum Stage {
			SEEKING_STATUS_LINE,
			SEEKING_HEADER,
			SEEKING_BODY,
			PROCESSING,
			DONE
		};

		enum State {
			AWAITING_MESSAGE,
			TREATING_MESSAGE
		};

		Request(void);
		Request(Server* server, std::string leftoverMessage);
		~Request(void);

		Response					&getResponse(void);
		std::string					getUntreatedMessage(void);
		Stage						getStage(void);
		State						getState(void);


		void						add(std::string buffer);
		void						parse(void);

	private:


		Stage												_stage;
		State												_state;

		Response											_response;

		Server*												_server;

		std::string											_untreatedMessage;
	
		Method												_method;
		std::string											_host;
		std::string											_path;
		std::map<std::string, std::string>					_arguments;
	
		std::map< std::string, std::vector<std::string> >	_fields;
		bool												_isBodyChunked;
		unsigned int										_bodyLength;
	
		std::string											_body;

		void						_parseStartLine(void);
		std::string					_findStartLine(void);
		int							_parseRequestLine(std::string startLine);
		int							_parseMethod(std::string methodString);

		int							_parseUri(std::string uri);
		int							_parseScheme(std::string scheme);
		int							_parseAuthority(std::string Authorityority);
		int							_parsePath(std::string path);
		int							_parseQuery(std::string query);
		
		int							_parseHTTPVer(std::string HTTPVerString);

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
		void						_parseFullBody(void);
		int							_parseChunkedBody(void);
		unsigned int				_findChunkLength(void);
		std::string					_findChunk(unsigned int chunkLength);


		void						_treat(void);

		static std::string			_toLower(std::string s);
		static bool					_isDelimiter(unsigned char c);
		static bool					_isVChar(unsigned char c);
		static bool					_isObsText(unsigned char c);
		static bool					_isHex(unsigned char c);
		static unsigned int			_stoi(std::string value);
		static unsigned int			_stoh(std::string value);

};

#endif
