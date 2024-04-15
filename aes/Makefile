CC = gcc
FLAG = -fPIC -shared
SRC = aes.c
OBJ = $(SRC:%.c=%.o)
NAME = libaes.so

all : $(NAME)


$(NAME) :
	$(CC) $(FLAG) $(NAME) $(SRC)

fclean:
	rm -rf $(NAME) $(OBJ)
	
re: fclean all