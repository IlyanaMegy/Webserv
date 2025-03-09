#ifndef CGI_HPP
# define CGI_HPP

#include <unistd.h>
#include <string>
#include <sys/wait.h>

#define PYTHON_PATH "/usr/bin/python3"

class	Epoll;

class CGI
{
	public:
		CGI(void);
		CGI(Epoll* epoll, std::string program, std::string cgi);
		~CGI(void);

		enum PipeEnd {
			READ_END,
			WRITE_END
		};

		int			getFd(void);
		bool		getHasSucceeded(void);
		std::string	getOutput(void);

		void	addOutput(std::string buffer);
		void	wait(void);

	private:
		std::string	_program;
		std::string	_cgi;
		char**		_arg;
		char**		_env;

		int	_pipeFd[2];
		int	_cpid;

		Epoll*		_epoll;

		bool		_hasSucceeded;
		std::string	_output;

		void	_launch(void);

};

#endif
