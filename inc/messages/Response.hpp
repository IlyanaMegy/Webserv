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

# define SERVERNAME "webserv"
# define ERRORPATH "default/errors/"
# define TIMEBUFFERSIZE 100

class Response
{
	public:
		Response(void);
		~Response(void);

		void	setStatusCode(std::string statusCode);
		void	setReasonMessage(std::string reasonMessage);
		void	setShouldClose(bool shouldClose);

		std::string	getMessage(void) const;
		bool		getShouldClose(void) const;
		bool		getIsComplete(void) const;

		void		createMessage(void);

		void		addCookies(std::vector<std::string>& cookies);

		void		fillError(std::string statusCode, std::string reasonMessage);
		void		fillGET(std::string path);
		void		fillDELETE(std::string path);
		void		fillPOST(std::string path, std::string body);

	private:
		std::string	_message;
		bool		_shouldClose;
		bool		_isComplete;

		std::string											_statusCode;
		std::string											_reasonMessage;
		std::map< std::string, std::vector<std::string> >	_fields;
		std::string											_content;

		std::string											_path;

		void	_createStatusLine(void);
		void	_createHeader(void);
		void	_createBody(void);

		void	_updateDate(void);

		void	_fillStatusLine(std::string statusCode, std::string reasonMessage);
		void	_fillHeader(void);
		int		_fillContent(std::string path);

		int		_deleteTarget(std::string path);

		int		_createTarget(std::string path, std::string body);

		static std::string	_itos(int value);

		std::string    _fixPath(std::string path);

};

#endif