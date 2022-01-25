NAME	=	webserv

SRCS	=	srcs/main.cpp \
			srcs/Handler/Request.cpp \
			srcs/Handler/Cgi.cpp \
			srcs/Utils/Utils.cpp \
			srcs/Utils/Uri.cpp \

OBJS	= 	$(SRCS:cpp=o)

OS 		=	$(shell lsb_release -si)
FLAGS	=	-std=c++98 -pedantic -fsanitize=address -Wall -Wextra -Werror $(if $(filter-out Ubuntu,$(OS)),,-I/usr/include/kqueue)
LDFLAGS =	$(if $(filter-out Ubuntu,$(OS)),,-lkqueue)

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