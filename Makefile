NAME	=	webserv

SRCS	=	srcs/main.cpp \
			srcs/Handler/Request.cpp \
			srcs/Handler/Response.cpp \
			srcs/Utils/Utils.cpp \
			srcs/Utils/Uri.cpp \

FLAGS	=	-std=c++98 -pedantic -Wall -Wextra -Werror -fsanitize=address -I/usr/include/kqueue
LDFLAGS =	-lkqueue
OBJS	= 	$(SRCS:cpp=o)

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