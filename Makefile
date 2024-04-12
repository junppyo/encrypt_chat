CC = clang
INC = -I./incs/
SERVER_SRCS = $(addprefix ./srcs/, utils.c user.c chat.c server.c)
CLIENT_SRCS = $(addprefix ./srcs/, client.c utils.c)
SERVER_OBJS = $(SERVER_SRCS:%.c=%.o)
CLIENT_OBJS = $(CLIENT_SRCS:%.c=%.o)
SERVER = server
CLIENT = client
LIB = -lkqueue

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_OBJS)
	$(CC) -g $(INC) $(SERVER_OBJS) -o $(SERVER) $(LIB)

$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(INC) $(CLIENT_OBJS) -o $(CLIENT) 

%.o: %.c
	$(CC) -g -c $(INC) $< -o $@

clean:
	rm -rf $(SERVER_OBJS) $(CLIENT_OBJS)

fclean:
	rm -rf $(SERVER_OBJS) $(CLIENT_OBJS)
	rm -rf $(SERVER) $(CLIENT)
re: fclean all
