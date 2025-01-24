NAME		:=	webserv

CONFIG		:=	Network/config/ServerConf.cpp \
				Network/config/ConfigParser.cpp

NETWORK		:=	Network/Server.cpp \
				Network/Client.cpp \
				Network/Epoll.cpp \
				Network/Socket.cpp

TOOLS		:=	tools/ParserTools.cpp \
				tools/Debugger.cpp

SRCS_DIR	:=	src

SRCS		:=	$(CONFIG) \
				$(NETWORK) \
				$(TOOLS) \
				main.cpp

SRCS		:=	${SRCS:%=${SRCS_DIR}/%}


OBJS_DIR	:=	obj

OBJS		:=	${SRCS:${SRCS_DIR}/%.cpp=${OBJS_DIR}/%.o}


CC			:=	c++
CFLAGS		:=	-Wall -Wextra -Werror -std=c++98
CPPFLAGS	:=	-I inc

RM			:=	${RM} -rf
DIR_DUP		=	mkdir -p ${@D}



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
