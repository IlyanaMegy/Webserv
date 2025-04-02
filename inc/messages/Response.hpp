#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <map>
# include <vector>
# include <ctime>
# include <fstream>
# include <iostream>
# include <sstream>
# include <unistd.h>
# include <set>
# include <sys/types.h>
# include <dirent.h>

# include "style.hpp"

# define SERVERNAME "webserv"
# define TIMEBUFFERSIZE 100

class	ServerConf;
class	CGI;

class Response
{
	public:
		Response(void);
		Response(ServerConf* conf);
		~Response(void);

		void	setStatusCode(std::string statusCode);
		void	setReasonMessage(std::string reasonMessage);
		void	setShouldClose(bool shouldClose);
		void	setVirtualPath(std::string virtualPath);
		void	setConf(ServerConf* conf);

		std::string	getMessage(void) const;
		bool		getShouldClose(void) const;
		bool		getIsComplete(void) const;

		void		createMessage(void);

		void		addCookies(std::vector<std::string>& cookies);

		void		fillError(std::string statusCode, std::string reasonMessage);
		void		fillGET(std::string path);
		void		fillDELETE(std::string path);
		void		fillPOST(std::string path, std::string body);
		void		fillCGI(CGI* cgi);
		void		fillRedir(std::string statusCode, std::string newHostname);
		void		fillAutoindex(std::string path);


		static std::string	itos(int value);

	private:
		std::string	_message;
		bool		_shouldClose;
		bool		_isComplete;

		ServerConf*											_conf;

		std::string											_statusCode;
		std::string											_reasonMessage;
		std::map< std::string, std::vector<std::string> >	_fields;
		std::string											_content;

		std::string											_virtualPath;

		void	_createStatusLine(void);
		void	_createHeader(void);
		void	_createBody(void);

		void	_updateDate(void);

		void	_fillStatusLine(std::string statusCode, std::string reasonMessage);
		void	_fillHeader(std::map< std::string, std::vector<std::string> > fields = std::map< std::string, std::vector<std::string> >());
		void	_fillErrorHeader(void);
		int		_fillContent(std::string path);
		int		_fillAutoindexPage(std::string path);

		int		_deleteTarget(std::string path);
		int		_createTarget(std::string path, std::string body);

		static std::map<std::string, std::string>	_initRedirMap(void);

};

#endif