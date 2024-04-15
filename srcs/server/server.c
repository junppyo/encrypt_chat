#include "../incs/server.h"

uint8_t KEY[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };

void print_event(struct kevent* event) {
    printf("event ident: %ld\n", event->ident);
    printf("event filter: %d\n", event->filter);
}

Server *InitServer(int port) {
    Server *server = (Server *)malloc(sizeof(Server));

    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_addr.s_addr = inet_addr(ADDRESS);
    server->server_addr.sin_port = htons(port);
    server->sock = socket(AF_INET, SOCK_STREAM, 0);
    server->aes = AesInit(KEY);
    server->status = 0;
    server->room_number = 0;
    server->db = DbInit();
    
    if (server->sock == -1) {
        printf("Socket create error\n");
        server->status = -1;
        return NULL;    
    }
    if (bind(server->sock, (struct sockaddr*)&server->server_addr, sizeof(struct sockaddr)) == -1) {
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
    server->read_fds = InitArray(sizeof(int));
    AddEvents(server, server->sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

    return server;
}


void AddEvents(Server *server, int sock, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
    struct kevent event;
    EV_SET(&event, sock, filter, flags, fflags, data, udata);
    InsertArray(server->changed, (void *)&event);
}

void Run(Server *server) {
    while (1) {
        static struct timespec ts;
        ts.tv_sec = 10;
        ts.tv_nsec = 0;
        int new_event = kevent(server->kqueue_fd, (struct kevent *)server->changed->data[0], server->changed->size, server->event_list, 10, &ts);
        ClearArray(server->changed);

        CheckEvent(server, new_event);
    }
    close(server->sock);
}

void CheckEvent(Server* server, int new_event) {
    struct kevent* current;
    int i;
    for (i = 0; i < new_event; i++) {
        current = &server->event_list[i];
        print_event(current);
        if (current->flags & EV_ERROR) {
            ErrorFlag(server, current);
            continue;
        }
        if (current->flags & EV_EOF) {
            DeleteUserByFd(server->users, current->ident);
            close(current->ident);
            continue;
        }
        if (current->filter == EVFILT_READ) {
            printf("read flag\n");
            ReadFlag(server, current);
        }
        if (current->filter == EVFILT_WRITE) {
            printf("write flag\n");
            WriteFlag(server, current);
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
    int client_sock;

    if ((int)event->ident == server->sock) {
        printf("connect client\n");
        client_sock = ConnectClient(server);
        if (client_sock < 0) {
            server->status = -1;
            return -1;
        }
    } else {
        User *user = UserByFd(server->users, event->ident);
        uint8_t *buf = user->buf;
        int len = recv(user->fd, buf + strlen(buf), BUF_SIZE, 0);

        if (len < 0) {
            printf("receive error\n");
            close(user->fd);
            DeleteUserByFd(server->users, user->fd);
            server->status = -1;
            return -1;
        }
        printf("\tbuf len : %ld\n", strlen(buf));
        printf("%s\n", buf);
        switch (user->status) {
            case WAIT_ID:
                printf("input_id\n");
                SetId(server, user, buf);
                break;
            case WAIT_PASS:
                break;
            case WAIT_REGIST:
                break;
            case LOGIN:
                break;
        }
        InsertArray(server->read_fds, (void*)&event->ident);
        memset(buf, 0, strlen(buf));
        
    }
    return 1;
}

int WriteFlag(Server *server, struct kevent *event) {
    User *user = UserByFd(server->users, event->ident);
    uint8_t *buf = user->buf;
    int len = strlen(buf);

    if (len > 0) {
        if (write(user->fd, buf, len) < 0) {
            printf("write error\n");
            server->status = -1;
            return -1;
        }
        printf("\twrited: %s\n", buf);
    }
    memset(buf, 0, BUF_SIZE);
    AddEvents(server, server->sock, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
    return 1;
}

int ConnectClient(Server *server) {
    int client_sock;
    char buf[BUF_SIZE] = {0, };

    if ((client_sock = accept(server->sock, NULL, NULL)) == -1) {
        printf("accept error\n");
        return -1;
    }
    fcntl(client_sock, F_SETFL, O_NONBLOCK);
    AddEvents(server, client_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    char *msg = "Please enter your ID\n";
    write(client_sock, msg, strlen(msg));
    // send(client_sock, msg, strlen(msg), 0);
    InsertArray(server->users, NewUser(client_sock));

    return client_sock;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("need port number\n");
        return -1;
    }
    Server *server = InitServer(atoi(argv[1]));
    
    Run(server);
    close(server->sock);
    return 0;
}