NAME		:=	webserv

SRCS_DIR	:=	src

SRCS		:=	Server.cpp \
				config/Socket.cpp \
				config/ServerConf.cpp \
				config/ConfigParser.cpp \
				Client.cpp \
				Epoll.cpp \
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
