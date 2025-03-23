#ifndef CGI_HPP
# define CGI_HPP

# include <unistd.h>
# include <string>
# include <sys/wait.h>
# include <map>
# include <string.h>
# include <vector>

#define PYTHON_PATH "/usr/bin/python3"
# define CLOSED -1

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

		int			getReadFd(void);
		int			getWriteFd(void);
		bool		getHasSucceeded(void);
		
		std::map< std::string, std::vector<std::string> >	getFields(void);
		std::string											getBody(void);
		
		void	closeWriteFd(void);
		void	addOutput(std::string buffer);
		void	wait(void);
		void	parse(void);

	private:
		std::string	_program;
		std::string	_cgi;

		std::map<std::string, std::string>	_env;
		char**								_envp;

		int	_pipeFdIn[2];
		int	_pipeFdOut[2];
		int	_cpid;

		Epoll*		_epoll;
		Client*		_client;
		Request*	_request;

		bool		_hasSucceeded;
		bool		_wasWaitedFor;
		std::string	_output;

		std::map< std::string, std::vector<std::string> >	_fields;
		std::string							_body;

		void		_initPipes(void);
		void		_setEnv(void);
		std::string	_createCookieString(std::vector<std::string> cookies);
		void		_convertEnv(void);
		void		_fillVar(char** varp, std::string key, std::string value);
		void		_launch(void);

		int			_parseHeaderFields(std::string header);
		int			_parseFieldLine(std::string fieldLine);

		int			_parseContentType(std::string contentType);
		int			_parseStatus(std::string status);

		static std::string	_findDirectory(std::string path);
		static std::string	_findName(std::string path);

		void	_closePipes(std::string method);

};

#endif
