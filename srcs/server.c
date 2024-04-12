#include "../incs/server.h"

void print_event(struct kevent* event) {
    printf("event ident: %d\n", event->ident);
    printf("event filter: %d\n", event->filter);
}

Server *InitServer() {
    Server *server = (Server *)malloc(sizeof(Server));

    server->server_addr.sin_family = AF_INET;
    // server->server_addr.sin_addr.s_addr = htonl("127.0.0.1");
    server->server_addr.sin_addr.s_addr = inet_addr(ADDRESS);
    server->server_addr.sin_port = htons(PORT);
    // server->sock = socket(AF_INET, SOCK_DGRAM, 0);
    server->sock = socket(AF_INET, SOCK_STREAM, 0);

    server->status = 0;
    server->room_number = 0;
    
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
    // server->changed = InitList(sizeof(struct kevent));
    // server->users = InitList(sizeof(User));
    // server->rooms = InitList(sizeof(Room));
    // server->read_fds = InitList(sizeof(int));
    AddEvents(server, server->sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

    return server;
}


void AddEvents(Server *server, int sock, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
    struct kevent event;
    printf("add events\n");
    EV_SET(&event, sock, filter, flags, fflags, data, udata);
    printf("add events2\n");
    InsertArray(server->changed, (void *)&event);
    printf("add events3\n");
    // InsertList(server->changed, (void *)&event);
}

void Run(Server *server) {
    int cnt = 0;
    while (1) {
        static struct timespec ts;
        ts.tv_sec = 10;
        ts.tv_nsec = 0;
        int new_event = kevent(server->kqueue_fd, (struct kevent *)server->changed->data[0], server->changed->size, server->event_list, 10, &ts);
        // int new_event = kevent(server->kqueue_fd, (struct kevent *)server->changed->head->next->data, GetSizeList(server->changed), server->event_list, 10, &ts);
        printf("new event: %d\n", new_event);
        // print_event((struct kevent *)server->changed->data[0]);
        ClearArray(server->changed);

        // EraseAllList(server->changed);
        CheckEvent(server, new_event);
        cnt++;
        // if (cnt == 4) break;
    }
    close(server->sock);
}

void CheckEvent(Server* server, int new_event) {
    struct kevent* current;
    int i;
    printf("size: %d\n", server->changed->size);
    // for (i = 0; i < server->changed->size; i++) {
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
        InsertArray(server->users, NewUser(client_sock));
        // InsertList(server->users, NewUser(client_sock));
    } else {
        User *user = UserByFd(server->users, event->ident);
        char *buf = user->buf;
        int len = recv(user->fd, buf + strlen(buf), BUF_SIZE, 0);

        if (len < 0) {
            printf("receive error\n");
            close(user->fd);
            DeleteUserByFd(server->users, user->fd);
            server->status = -1;
            return -1;
        }
        printf("buf: %s\n", buf);
        InsertArray(server->read_fds, (void*)&event->ident);
        // InsertList(server->read_fds, (void*)event->ident);
    }
    return 1;
}

int WriteFlag(Server *server, struct kevent *event) {
    User *user = UserByFd(server->users, event->ident);
    char *buf = user->buf;
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

    if ((client_sock = accept(server->sock, NULL, NULL)) == -1) {
        printf("accept error\n");
        return -1;
    }
    fcntl(client_sock, F_SETFL, O_NONBLOCK);
    AddEvents(server, client_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

    return client_sock;
}

int main() {
    Server *server = InitServer();

    Run(server);    
    close(server->sock);
    return 0;
}
