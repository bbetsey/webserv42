NAME	=	test/webserv

SRCS	=	srcs/main.cpp \
			srcs/Handler/Request.cpp \
			srcs/Handler/Cgi.cpp \
			srcs/Utils/Utils.cpp \
			srcs/Utils/Uri.cpp \
			srcs/Config/Config.cpp \
			srcs/Config/Parser.cpp \
			srcs/Network/Network.cpp \
			srcs/Network/Socket.cpp \
			srcs/Utils/Logger.cpp \
			srcs/Utils/AutoIndexGen.cpp

OBJS	= 	$(SRCS:cpp=o)

OS 		=	$(uname)
FLAGS	=	-std=c++98 -pedantic -Wall -Wextra -Werror -I/usr/include/kqueue
# LDFLAGS =	-lkqueue

all: $(NAME)

$(NAME): $(OBJS)
	clang++ $(FLAGS) -o $(NAME) $(SRCS) $(LDFLAGS)

%.o: %.cpp
	clang++ $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all