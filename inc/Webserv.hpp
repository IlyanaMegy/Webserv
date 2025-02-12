#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define BUFFER_SIZE 42
#define MAXCONNECT 30
#define MAX_CONTENT_LENGTH 30000000

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

// #include "Network/config/ServerConf.hpp"
// #include "Network/config/ConfigParser.hpp"
// #include "Network/Client.hpp"
// #include "Network/Epoll.hpp"
// #include "Network/Server.hpp"
#include "tools/Debugger.hpp"
#include "tools/ParserTools.hpp"
#include "tools/Singletons.hpp"
#include "tools/style.hpp"

#endif
