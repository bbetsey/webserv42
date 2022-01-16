NAME	=	webserv

SRCS	=	srcs/Parser/main.cpp \
			srcs/Parser/Request.cpp \
			srcs/Parser/Response.cpp \
			srcs/Utils/Split.cpp \

FLAGS	=	-std=c++98 -pedantic -Wall -Wextra -Werror -fsanitize=address
OBJS	= 	$(SRCS:cpp=o)

all: $(NAME)

$(NAME): $(OBJS)
	clang++ $(FLAGS) -o $(NAME) $(SRCS)

%.o: %.cpp
	clang++ $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all