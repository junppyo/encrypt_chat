#include "../../incs/room.h"

char *GetTime() {
    time_t timer;
    struct tm* t;
    char *ret = malloc(sizeof(char) * 19);
    if (!ret) return NULL;
    timer = time(NULL);
    t = localtime(&timer);

    sprintf(ret, "%02d-%02d-%02d %02d:%02d:%02d ", t->tm_year - 100, t->tm_mon + 1,
                                                   t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    return ret;
}

Room *InitRoom(Server *server, char *roomname, char *password) {
    Room *room = malloc(sizeof(Room));
    char *filename = malloc(sizeof(char) * 40);

    if (!room || !filename) {
         return NULL;
    }
    strcpy(room->name, roomname);
    // if (!password) {
    printf("password : %s\n", password);
    printf("pass len : %d\n", strlen(password));
    if (strlen(password) == 0) {
        room->is_secret = false;
    }
    else {
        room->is_secret = true;
        strcpy(room->password, password);
    }

    room->user_fds = InitArray(sizeof(int));
    if (!room->user_fds) return NULL;
    printf("\t Init Room\n");
    printf("datasize : %d\tsize: %d\n", room->user_fds->data_size, room->user_fds->size);
    
    char *gettime = GetTime();
    sprintf(filename, "%s%s.log", gettime, roomname);
    room->log_fd = fopen(filename, "w");
    InsertArray(server->rooms, (void *)room);
    room->number = server->rooms->size;

    free(gettime);
    free(filename);
    return room;
}

void PrintRoomList(Array *rooms, User *user) {
    int i, fd;

    fd = user->fd;
    for (i = 0; i < rooms->size; i++) {
        Room *room = rooms->data[i];
        if (room->is_secret) {
            char *tmp = Strcat("*", room->name);
            printf("private room : %s\n", tmp);
            write(fd, tmp, strlen(tmp));
            free(tmp);
        } else {
            write(fd, room->name, strlen(room->name));
        }
        write(fd, " ", 1);
    }
    write(fd, "\n", 1);
}

Room *FindRoomByName(Array *rooms, char *name) {
    int i;

    for (i = 0; i < rooms->size; i++) {
        Room *room = rooms->data[i];
        if (!strcmp(room->name, name)) return room;
    }
    return NULL;
}

Room *FindRoomByNumber(Array *rooms, int n) {
    int i;
    printf("find n : %d , size : %d\n", n, rooms->size);
    for (i = 0; i < rooms->size; i++) {
        Room *room = rooms->data[i];
        if (room->number == n) return room;
    }
    return NULL;
}

Room *MakeRoom(Server *server, char *buf) {
    printf("Make room\n");
    int i;
    Room *room;
    for (i = 0; i < strlen(buf); i++) {
        if (buf[i] == '/') break;
    }
    if (i == strlen(buf)) {
        if (i > 16) {
            return NULL;
        }
        room = InitRoom(server, buf, "");
    }
    else {
        char *name = substr(buf, 0, i);
        char *pass = substr(buf, i + 1, strlen(buf));
        printf("%d room name : %s     room pass : %s\n", room->number, name, pass);

        if (!name || !pass) return NULL;
        room = InitRoom(server, name, pass);
        free(name);
        name = NULL;
        free(pass);
        pass = NULL;
    }

    return room;
}

int JoinRoom(Server *server, User *user, char *name) {
    Room *room = FindRoomByName(server->rooms, name);
    
    if (room == NULL) {
        room = MakeRoom(server, name);
        if (!room) return 0;
    } else {
        printf("Join room\n");
        if (room->is_secret) {
            RequestRoomPass(room, user);
            return 1;
        }
    }
    user->status = PUBLIC;
    InsertArray(room->user_fds, &user->fd);
    printf("\tuser %d join room number %d\n", user->fd, user->room_number);
    return 0;
}

int LeaveRoom(Server *server, User *user) {
    int i, j;
    Room *room = FindRoomByNumber(server->rooms, user->room_number);
    printf("Leave Room %s\n", room->name);
    for (i = 0; i < room->user_fds->size; i++) {
        int *tmp = room->user_fds->data[i];
        if (*tmp == user->fd) {
            printf("\tuser %d leave room number %d\n", user->fd, room->number);
            EraseArray(room->user_fds, i);
            break;
        }
    }
    if (room->user_fds->size == 0) {
        printf("Delete Room %s\n", room->name);
        for (i = 0; i < server->rooms->size; i++) {
            Room *tmp = server->rooms->data[i];
            if (tmp->number == room->number) {
                printf("\tDelete %d %s room\n", tmp->number, tmp->name);
                EraseArray(server->rooms, i);
                break;
            }
        }

    }
    
    return 0;
}

int RequestRoomPass(Room *room, User *user) {
    char *msg = "This is private room.\nPlease enter room password : ";
    printf("request password\n");
    
    write(user->fd, msg, strlen(msg));
    user->room_number = room->number;
    user->status = TRY_PRIVATE;
}

int TryPrivateRoom(Server *server, User *user) {
    printf("try private room\n");
    Room *room = FindRoomByNumber(server->rooms, user->room_number);
    
    if (!strcmp(user->buf, room->password)) {
        if (!InsertArray(room->user_fds, &user->fd)) {
            user->status = LOGIN;
            printf("insert to user_fds failed\n");
             return false;
        }
        user->status = PRIVATE;
        printf("\tuser %d join private room number %d\n", user->fd, user->room_number);
        printf("succes");
        return true;
    } else {
        user->status = LOGIN;
    }
    return false;
}
