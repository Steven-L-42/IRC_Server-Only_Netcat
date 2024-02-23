NAME			:= ircserv

CXX				:= c++

CXXFLAGS		:= -Wall -Wextra -Werror -std=c++98

SRCS			:= main.cpp Server.cpp

OBJS			:= $(SRCS:.cpp=.o)

$(NAME):	$(OBJS)
			@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
			@make clean

all:		$(NAME)

clean:
			@rm -rf $(OBJS)

fclean:		clean
			@rm -rf $(NAME)

re: 		fclean all