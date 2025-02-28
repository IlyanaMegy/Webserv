#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define BUFFER_SIZE 42
#define MAXCONNECT 30
#define MAX_CONTENT_LENGTH 30000000

enum Method { GET, POST, DELETE };

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "Debugger.hpp"
#include "ParserTools.hpp"
#include "Singletons.hpp"
#include "style.hpp"

#endif
