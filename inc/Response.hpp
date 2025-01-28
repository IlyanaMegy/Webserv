#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>

class Response
{
	public:
		Response(void);
		~Response(void);

		void	setStatusCode(std::string statusCode);

		std::string	getStatusCode(void) const;
		std::string	getMessage(void) const;

		void	createMessage(void);

	private:
		std::string	_message;

		std::string	_statusCode;
};

#endif