#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include <vector>
# include <cctype>
# include <iostream>
# include <algorithm>

# include "Response.hpp"

# include "style.hpp"

# define MAXSTATUSLINEOCTETS 2000
# define MAXHEADEROCTETS 4000
# define MAXBODYOCTETS 2000000000

class	Server;
class	CGI;
class	Epoll;
class	Client;
class	ServerConf;

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
		Request(Server* server, Client* client, Epoll* epoll, std::string leftoverMessage);
		~Request(void);

		Response											&getResponse(void);
		std::string											getUntreatedMessage(void);
		Stage												getStage(void);
		State												getState(void);
		CGI*												getCGI(void);
		Server*												getServer(void);
		Client*												getClient(void);
		ServerConf*											getConf(void);
		Method												getMethod(void);
		std::string											getPath(void);
		std::string											getQuery(void);
		std::map< std::string, std::vector<std::string> >&	getFields(void);
		unsigned int										getBodyLength(void);
		std::string											getBody(void);
		
		
		void						add(std::string buffer);
		void						parse(void);

		void						treat(void);
		void						treatCGI(void);
		
		static int					parseFieldName(std::string fieldName);
		static int					parseFieldValue(std::string fieldValue);
		static int					parseContentLength(std::string contentLength);
		
		static std::string			toLower(std::string s);
		static bool					isToken(std::string s);
		static bool					isDelimiter(unsigned char c);
		static bool					isVChar(unsigned char c);
		static unsigned int			stoi(std::string value);

	private:


		Stage												_stage;
		State												_state;

		Response											_response;

		Server*												_server;
		ServerConf*											_conf;
		Client*												_client;
		Epoll*												_epoll;
		CGI*												_cgi;

		std::string											_untreatedMessage;
	
		Method												_method;
		std::string											_host;
		std::string											_path;
		std::string											_query;
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
		int							_parseCompletedFields(void);
		int							_findTransferEncoding(void);
		int							_findContentLength(void);
		int							_findHost(void);
		void						_findConnection(void);
		void						_findCookies(void);

		void						_parseBody(void);
		void						_parseFullBody(void);
		int							_parseChunkedBody(void);
		unsigned int				_findChunkLength(void);
		std::string					_findChunk(unsigned int chunkLength);

		void						_treatPOST(std::string physicalPath, struct stat sb, int res);
		void						_treatReg(void);
		void						_treatDir(void);

		int							_launchCGI(std::string physicalPath);

		static bool					_isObsText(unsigned char c);
		static bool					_isHex(unsigned char c);
		static unsigned int			_stoh(std::string value);
		static void					_split(std::vector<std::string>& vector);

};

#endif
