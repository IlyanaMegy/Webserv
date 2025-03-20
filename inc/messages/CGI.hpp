#ifndef CGI_HPP
# define CGI_HPP

#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <map>
#include <string.h>
#include <vector>

#define PYTHON_PATH "/usr/bin/python3"

class	Epoll;
class	Client;
class	Request;

class CGI
{
	public:
		CGI(void);
		CGI(Epoll* epoll, Request* request, std::string program, std::string cgi);
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

		std::map<std::string, std::string>	_env;
		char**								_envp;

		int	_pipeFd[2];
		int	_cpid;

		Epoll*		_epoll;
		Client*		_client;
		Request*	_request;

		bool		_hasSucceeded;
		bool		_wasWaitedFor;
		std::string	_output;

		void		_setEnv(void);
		std::string	_createCookieString(std::vector<std::string> cookies);
		void		_convertEnv(void);
		void		_fillVar(char** varp, std::string key, std::string value);
		void		_launch(void);

		static std::string	_findDirectory(std::string path);
		static std::string	_findName(std::string path);

};

#endif
