NAME = Server

CC =  c++

RM = 		@rm -rf


FLAGS = -g -Wall -Werror -Wextra -std=c++98 -pedantic # -fsanitize=address

CFILES =	main.cpp \
			Server.cpp \



OBJECTS = 	$(CFILES:.cpp=.o)

$(NAME): 	$(OBJECTS)
			@$(CC) $(FLAGS) $(OBJECTS) -o $(NAME)

all: $(NAME)

clean:
	$(RM) ./*.o

fclean: clean
	$(RM) $(NAME)

re: fclean all

.cpp.o:
	${CC} ${FLAGS} -c $< -o ${<:.cpp=.o}

.PHONY: re fclean clean all
