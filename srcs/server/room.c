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
    Room *room = NewElement(server->rooms);
    char *filename = malloc(sizeof(char) * 40);
    uint8_t *encrypt;
    if (!room || !filename) {
         return NULL;
    }
    
    strcpy(room->name, roomname);
    if (strlen(password) == 0) {
        room->is_secret = false;
        room->aes = NULL;
        encrypt = NULL;
    }
    else {
        room->is_secret = true;
        strcpy(room->password, password);
        encrypt = Encrypt(server->aes, password);
        room->aes = AesInit(encrypt);
    }

    room->user_fds = InitArray(sizeof(int));
    if (!room->user_fds) return NULL;
    
    char *gettime = GetTime();
    sprintf(filename, "%s%s.log", gettime, roomname);
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    room->log_fd = open(filename, O_CREAT | O_WRONLY, S_IRWXO);
    room->number = server->rooms->size + 1;
    char *tmp = ToHex(encrypt);
    DbCreateLog(server->db, filename, tmp);

    if (tmp)
        free(tmp);
    free(gettime);
    free(filename);
    if (encrypt)
        free(encrypt);
    return room;
    // return ret;
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
    char *msg = "Please input the room name : ";
    write(fd, msg, strlen(msg));
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

        if (!name || !pass) return NULL;
        room = InitRoom(server, name, pass);
        free(name);
        name = NULL;
        free(pass);
        pass = NULL;
    }

    return room;
}

int JoinRoom(Server *server, User *user, char *buf) {
    int i;
    char *name;
    char *pass;
    Room *room;

    printf("join room : %s\n", buf);
    for (i = 0; i < strlen(buf); i++) {
        if (buf[i] == '/') break;
    }
    if (i == strlen(buf)) {
        if (i > 16) {
            return 1;
        }
        name = substr(buf, 0, i);
        pass = substr(buf, i, strlen(buf));
    } else {
        name = substr(buf, 0, i);
        pass = substr(buf, i + 1, strlen(buf));
    }
    if (!name || !pass) return 1;

    printf("name : %s\npass: %s\n", name, pass);
    room = FindRoomByName(server->rooms, name);
    
    if (!room) {
        printf("Make room\n");
        room = InitRoom(server, name, pass);
        free(name);
        if (pass) free(pass);
        if (!room) return 1;
        user->room_number = room->number;
        int *tmp = NewElement(room->user_fds);
        *tmp = user->fd;
        if (room->is_secret) {
            user->status = PRIVATE;
            write(user->fd, room->aes->key, 16);
        }
        else {
            user->status = PUBLIC;
            write(user->fd, "JOIN", 4);
        }
        return 0;
    } else {
        printf("Join room\n");
        user->room_number = room->number;
        if (room->is_secret) {
            if (strlen(pass) != 0) {
                printf("immediately join private \n");
                strcpy(user->buf, pass);
                free(name);
                free(pass);
                name = NULL;
                pass = NULL;
                return !TryPrivateRoom(server, user);
            } else {
                printf("room number : %ld\n", room->number);
                RequestRoomPass(room, user);
            }
        } else {
            user->status = PUBLIC;
            write(user->fd, "JOIN", 4);
            int *fd = NewElement(room->user_fds);
            *fd = user->fd;
            printf("\tuser %d join room number %ld\n", user->fd, user->room_number);
        }
        free(name);
        free(pass);
        name = NULL;
        pass = NULL;
    }
    return 0;
}

int LeaveRoom(Server *server, User *user) {
    int i, j;
    printf("LeaveRoom start\n");
    printf("user room_number : %ld\n", user->room_number);
    Room *room = FindRoomByNumber(server->rooms, user->room_number);
    printf("Leave Room %s\n", room->name);
    for (i = 0; i < room->user_fds->size; i++) {
        int *tmp = room->user_fds->data[i];
        if (*tmp == user->fd) {
            printf("\tuser %d leave room number %ld\n", user->fd, room->number);
            EraseArray(room->user_fds, i);
            break;
        }
    }
    printf("room user_fds size : %ld\n", room->user_fds->size);
    if (room->user_fds->size == 0) {
        printf("Delete Room %s\n", room->name);
        for (i = 0; i < server->rooms->size; i++) {
            Room *tmp = server->rooms->data[i];
            if (tmp->number == room->number) {
                printf("\tDelete %ld %s room\n", tmp->number, tmp->name);
                close(room->log_fd);
                if (room->aes) free(room->aes);
                FreeArray(room->user_fds);
                EraseArray(server->rooms, i);
                break;
            }
        }
    }
    user->status = LOGIN;
    printf("LeaveRoom end\n");
    
    return 0;
}

void RequestRoomPass(Room *room, User *user) {
    char *msg = "This is private room.\nPlease enter room password : ";
    printf("request password\n");
    
    write(user->fd, msg, strlen(msg));
    user->room_number = room->number;
    user->status = TRY_PRIVATE;
}

int TryPrivateRoom(Server *server, User *user) {
    printf("try private room\n");
    Room *room = FindRoomByNumber(server->rooms, user->room_number);
    // char *tmp = ToString(room->password);
    // char *pass = Decrypt(server->aes, tmp, 16);
    // printf("room pass : %s\n", pass);
    int *fds;

    if (!strcmp(user->buf, room->password)) {
        if (!(fds = NewElement(room->user_fds))) {
            user->status = LOGIN;
            printf("insert to user_fds failed\n");
            return false;
        }
        *fds = user->fd;
        user->status = PRIVATE;
        printf("room KEY : %s\n", room->password);
        write(user->fd, room->aes->key, 16);
        return true;
    } else {
        user->status = LOGIN;
    }
    return false;
}

void ClearRooms(Array *rooms) {
    int i;

    for (i = 0; i < rooms->size; i++) {
        Room *room = rooms->data[i];
        if (room->aes) free(room->aes);
        ClearArray(room->user_fds);
        free(room->user_fds);
    }
    ClearArray(rooms);
}