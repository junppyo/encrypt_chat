CC = gcc
INC = -I./incs/
SRCS = $(addprefix ./srcs/, utils.c)
SERVER_SRCS = $(addprefix ./srcs/server/, user.c chat.c server.c db.c command.c room.c)
CLIENT_SRCS = $(addprefix ./srcs/client/, client.c)
READER_SRCS = $(addprefix ./srcs/, log_reader.c)
OBJS = $(SRCS:%.c=%.o)
SERVER_OBJS = $(SERVER_SRCS:%.c=%.o)
CLIENT_OBJS = $(CLIENT_SRCS:%.c=%.o)
READER_OBJS = $(READER_SRCS:%.c=%.o)
SERVER = server
CLIENT = client
READER = reader
LIB = -lkqueue -lmysqlclient
AES_DIR = ./aes
AES_FLAG = -I./aes/aes.h -L./ -laes
AES = libaes.so

all: $(AES) $(SERVER) $(CLIENT) $(READER)
	
$(AES): $(AES_DIR)/aes.o
	$(MAKE) -C $(AES_DIR)
	cp $(AES_DIR)/$(AES) ./$(AES)
	sudo cp ./$(AES) /lib/

$(SERVER): $(SERVER_OBJS) $(OBJS)
	$(CC) $(INC) $(OBJS) $(SERVER_OBJS) -o $(SERVER) $(LIB) $(AES_FLAG)
# $(CC) -g -fsanitize=address $(INC) $(OBJS) $(SERVER_OBJS) -o $(SERVER) $(LIB) $(AES_FLAG)

$(CLIENT): $(CLIENT_OBJS) $(OBJS)
	$(CC) $(INC) $(OBJS) $(CLIENT_OBJS) -o $(CLIENT) $(AES_FLAG)
# $(CC) -g -fsanitize=address $(INC) $(OBJS) $(CLIENT_OBJS) -o $(CLIENT) $(AES_FLAG)

$(READER) : $(READER_OBJS) $(OBJS)
	$(CC) $(INC) $(OBJS) $(READER_OBJS) -o $(READER) -lmysqlclient $(AES_FLAG)
# $(CC) -g -fsanitize=address $(INC) $(OBJS) $(READER_OBJS) -o $(READER) -lmysqlclient $(AES_FLAG)

%.o: %.c
	$(CC) -c $(INC) $< -o $@
# $(CC) -g -fsanitize=address  -c $(INC) $< -o $@

clean:
	rm -rf $(SERVER_OBJS) $(CLIENT_OBJS) $(READER_OBJS)

fclean:
	$(MAKE) -C $(AES_DIR) fclean
	rm -rf $(SERVER_OBJS) $(CLIENT_OBJS) $(READER_OBJS) $(OBJS)
	rm -rf $(SERVER) $(CLIENT) $(READER) $(AES) 
	sudo rm -rf *.log*
	
re: fclean all
