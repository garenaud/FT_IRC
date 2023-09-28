NAME = Server

CC =  c++

RM = 		@rm -rf


FLAGS = -Wall -Werror -Wextra -std=c++98 #-pedantic -fsanitize=address

CFILES =	main.cpp \
			srcs/User.cpp \
			srcs/Server.cpp \
			srcs/Msg.cpp \



OBJECTS = 	$(CFILES:.cpp=.o)

$(NAME): 	$(OBJECTS)
			@$(CC) $(FLAGS) $(OBJECTS) # -o $(NAME)

all: $(NAME)

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.cpp.o:
	${CC} ${FLAGS} -c $< -o ${<:.cpp=.o}

.PHONY: re fclean clean all