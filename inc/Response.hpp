#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>

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

		std::string	_statusCode;
		std::string _reasonMessage;

		void	_createStatusLine(void);
		void	_createHeader(void);
		void	_createBody(void);

};

#endif