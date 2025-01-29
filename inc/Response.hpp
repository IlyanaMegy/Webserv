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

		std::string	getStatusCode(void) const;
		std::string	getMessage(void) const;

		void	createMessage(void);

	private:
		std::string	_message;

		std::string											_statusCode;
		std::string											_reasonMessage;
		std::map< std::string, std::vector<std::string> >	_fields;

		void	_createStatusLine(void);
		void	_createHeader(void);
		void	_createBody(void);

		void	_updateDate(void);

};

#endif