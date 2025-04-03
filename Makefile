NAME			:=	webserv


SRCS_DIR		:=	src

SRCS_CONFIG		:=	ConfigParser.cpp \
					Location.cpp \
					ServerConf.cpp \
					ParserTools.cpp

SRCS_MESSAGES	:=	Request.cpp \
					Response.cpp \
					CGI.cpp

SRCS_NETWORK	:=	Client.cpp \
					Epoll.cpp \
					Server.cpp \
					Socket.cpp

SRCS			:=	${addprefix config/, ${SRCS_CONFIG}} \
					${addprefix network/, ${SRCS_NETWORK}} \
					${addprefix messages/, ${SRCS_MESSAGES}} \
					Signal.cpp \
					ServerMonitor.cpp \
					main.cpp

SRCS			:=	${SRCS:%=${SRCS_DIR}/%}


OBJS_DIR		:=	obj

OBJS			:=	${SRCS:${SRCS_DIR}/%.cpp=${OBJS_DIR}/%.o}


CC				:=	c++
CFLAGS			:=	-Wall -Wextra -Werror -std=c++98
CPPFLAGS		:=	-I inc -I inc/config -I inc/network -I inc/messages

RM				:=	${RM} -rf
DIR_DUP			=	mkdir -p ${@D}



all: ${NAME}

${NAME}: ${OBJS}
	${CC} $^ -o $@

${OBJS_DIR}/%.o: ${SRCS_DIR}/%.cpp
	${DIR_DUP}
	${CC} ${CFLAGS} ${CPPFLAGS} -c -o $@ $<

clean:
	${RM} ${OBJS_DIR}

fclean: clean
	${RM} ${NAME}

re: fclean
	${MAKE} all

.PHONY: clean fclean re
