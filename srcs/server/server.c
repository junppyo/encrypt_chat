#include "../incs/server.h"

uint8_t KEY[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
volatile sig_atomic_t Stop;

void print_event(struct kevent* event) {
    printf("event ident: %ld\n", event->ident);
    printf("event filter: %d\n", event->filter);
}

Server *InitServer(int port) {
    Server *server = (Server *)malloc(sizeof(Server));
    struct sockaddr_in server_addr;
    int optval = 1;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ADDRESS);
    server_addr.sin_port = htons(port);
    server->sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    server->aes = AesInit(KEY);
    server->status = 0;
    server->db = DbInit();

    if (server->sock == -1) {
        printf("Socket create error\n");
        server->status = -1;
        return NULL;    
    }
    if (bind(server->sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1) {
        printf("Socket bind error\n");
        perror(strerror(errno));
        return NULL;    
    }
    if (listen(server->sock, 5) == -1) {
        printf("Socket listen error\n");
        perror(strerror(errno));
        return NULL;    
    }

    server->kqueue_fd = kqueue();
    if (server->kqueue_fd == -1) {
        printf("kqueue error\n");
        perror(strerror(errno));
        return NULL;    
    }
    printf("socket created\n");
    server->changed = InitArray(sizeof(struct kevent));
    server->users = InitArray(sizeof(User));
    server->rooms = InitArray(sizeof(Room));
    AddEvents(server, server->sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

    return server;
}


void AddEvents(Server *server, int sock, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
    struct kevent event;
    EV_SET(&event, sock, filter, flags, fflags, data, udata);
    struct kevent *tmp = NewElement(server->changed);
    memcpy(tmp, &event, sizeof(struct kevent));
}

void Run(Server *server) {
    int i;
    static struct timespec ts;

    while (!Stop && !server->status) {
        ts.tv_sec = 10;
        ts.tv_nsec = 0;
        int new_event = kevent(server->kqueue_fd, (struct kevent *)server->changed->data[0], server->changed->size, server->event_list, 10, &ts);
        ClearArray(server->changed);
        CheckEvent(server, new_event);
    }
    CloseServer(server);
}

void CloseServer(Server *server) {
    close(server->kqueue_fd);
    FreeArray(server->changed);
    FreeUsers(server->users);
    FreeRooms(server->rooms);
    mysql_close(server->db);
    mysql_library_end();
    free(server->aes);
    close(server->sock);
    free(server);
}

void CheckEvent(Server* server, int new_event) {
    struct kevent* current;
    int i;

    for (i = 0; i < new_event; i++) {
        current = &server->event_list[i];
        print_event(current);
        if (current->flags & EV_ERROR) {
            ErrorFlag(server, current);
        }
        else if (current->flags & EV_EOF) {
            printf("disconnect user\n");
            DisconnectUser(server, current->ident);
            DeleteUserByFd(server->users, current->ident);
            close(current->ident);
        }
        else if (current->filter == EVFILT_READ) {
            printf("read flag\n");
            ReadFlag(server, current);
        }
    }
}

int ErrorFlag(Server* server, struct kevent *event) {
    server->status = -1;

    if (event->ident == server->sock) 
        printf("server socket error\n");
    else
        printf("client socket error\n");

    return -1;
}


int ReadFlag(Server *server, struct kevent *event) {
    int client_sock, len, *fd;
    uint8_t *buf;
    User *user;
    unsigned char *msg;

    if ((int)event->ident == server->sock) {
        printf("connect client\n");
        client_sock = ConnectClient(server);
        if (client_sock < 0) {
            server->status = -1;
            return -1;
        }
    } else {
        user = UserByFd(server->users, event->ident);
        buf = user->buf;
        len = read(user->fd, buf, BUF_SIZE);
        printf("\t\trecv : %d bytes\n", len);
        user->buf_len = len;

        if (len < 0) {
            printf("receive error\n");
            close(user->fd);
            DeleteUserByFd(server->users, user->fd);
            server->status = -1;
            return -1;
        }
        printf("user status : %d\n", user->status);
        switch (user->status) {
            case WAIT_ID:
                SetId(server, user, buf);
                break;
            case WAIT_PASS:
                if (!TryLogin(server, user, buf)) {
                    close(user->fd);
                    DeleteUserByFd(server->users, user->fd);
                } else {
                    user->status = LOBBY;
                }
                break;
            case WAIT_REGIST:
                if (CreateUser(server, user, buf)) {
                    close(user->fd);
                    DeleteUserByFd(server->users, user->fd);
                } else {
                    user->status = LOBBY;
                }
                break;
            case LOBBY:
                if (JoinRoom(server, user, user->buf)) {
                    msg = "Create or Join Failed\n";
                    printf("%s", msg);
                    write(user->fd, msg, strlen(msg));
                    PrintRoomList(server->rooms, user);
                }
                break;
            case TRY_PRIVATE:
                printf("try private\n");
                if (!TryPrivateRoom(server, user)) {
                    printf("try private fail\n");
                    PrintRoomList(server->rooms, user);
                }
                break;
            default:
                printf("default flags : %d\n", user->status);
                SendMsg(server, user);
                break;
        }
        memset(buf, 0, strlen(buf));        
    }
    return 1;
}

int ConnectClient(Server *server) {
    int client_sock;
    unsigned char *msg, buf[BUF_SIZE] = {0, };

    if ((client_sock = accept(server->sock, NULL, NULL)) == -1) {
        printf("accept error\n");
        return -1;
    }
    fcntl(client_sock, F_SETFL, O_NONBLOCK);
    AddEvents(server, client_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    msg = "Please enter your ID : ";
    write(client_sock, msg, strlen(msg));
    InsertArray(server->users, NewUser(client_sock));

    return client_sock;
}

void ctrlc_handler(int signum) {
    Stop = 1;
}

int main(int argc, unsigned char *argv[]) {
    if (argc < 2) {
        printf("need port number\n");
        return -1;
    }
    signal(SIGINT, ctrlc_handler);
    Server *server = InitServer(atoi(argv[1]));
    
    Run(server);
    return 0;
}
