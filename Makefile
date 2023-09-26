NAME = ircserv

SRC = main.cpp Channel.cpp Client.cpp Server.cpp Commands.cpp

OBJS = $(SRC:.cpp=.o)

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98

.cpp.o:
	@$(CC) $(CFLAGS) -c $< -o $(<:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo  "Compiling..."

clean:
	@rm -rf $(OBJS)
	@echo "Removing files..."

fclean:
	@rm -rf $(OBJS)
	@rm -rf $(NAME)
	@echo "Removing files..."

re: fclean all

.PHONY: all clean fclean re