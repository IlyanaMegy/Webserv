#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <map>
# include <vector>
# include <ctime>

# define SERVERNAME "webserv"
# define TIMEBUFFERSIZE 100

class Response
{
	public:
		Response(void);
		~Response(void);

		void	setStatusCode(std::string statusCode);
		void	setReasonMessage(std::string reasonMessage);

		std::string	getMessage(void) const;
		bool		getShouldClose(void) const;
		bool		getIsComplete(void) const;

		void		createMessage(void);

		void		fillError(std::string statusCode, std::string reasonMessage);

	private:
		std::string	_message;
		bool		_shouldClose;
		bool		_isComplete;

		std::string											_statusCode;
		std::string											_reasonMessage;
		std::map< std::string, std::vector<std::string> >	_fields;

		std::string											_path;

		void	_createStatusLine(void);
		void	_createHeader(void);
		void	_createBody(void);

		void	_updateDate(void);
		void	_updateConnection(void);

};

#endif