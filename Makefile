CC = gcc
INC = -I./incs/
SERVER_SRCS = $(addprefix ./srcs/server/, utils.c user.c chat.c server.c db.c command.c)
CLIENT_SRCS = $(addprefix ./srcs/client/, client.c)
SERVER_OBJS = $(SERVER_SRCS:%.c=%.o)
CLIENT_OBJS = $(CLIENT_SRCS:%.c=%.o)
SERVER = server
CLIENT = client
LIB = -lkqueue -lmysqlclient
AES_DIR = ./aes
AES_FLAG = -I./aes/aes.h -L./ -laes
AES = libaes.so

all: $(AES) $(SERVER) $(CLIENT)
	
$(AES): $(AES_DIR)/aes.o
	$(MAKE) -C $(AES_DIR)
	cp $(AES_DIR)/$(AES) ./$(AES)
	sudo cp ./$(AES) /lib/

$(SERVER): $(SERVER_OBJS)
	$(CC) -g $(INC) $(SERVER_OBJS) -o $(SERVER) $(LIB) $(AES_FLAG)

$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(INC) $(CLIENT_OBJS) -o $(CLIENT) $(AES_FLAG)

%.o: %.c
	$(CC) -g -c $(INC) $< -o $@

clean:
	rm -rf $(SERVER_OBJS) $(CLIENT_OBJS)

fclean:
	$(MAKE) -C $(AES_DIR) fclean
	rm -rf $(SERVER_OBJS) $(CLIENT_OBJS)
	rm -rf $(SERVER) $(CLIENT) $(AES)
	
re: fclean all
