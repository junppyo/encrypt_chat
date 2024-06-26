#include "../../incs/room.h"

unsigned char *GetTime() {
    time_t timer;
    struct tm* t;
    unsigned char *ret = malloc(sizeof(unsigned char) * 19);

    if (!ret) return NULL;
    timer = time(NULL);
    t = localtime(&timer);

    sprintf(ret, "%02d-%02d-%02d_%02d:%02d:%02d_", t->tm_year - 100, t->tm_mon + 1,
                                                   t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    return ret;
}

Room *InitRoom(Server *server, unsigned char *roomname, unsigned char *password) {
    Room *room = NewElement(server->rooms);
    mode_t mode = S_IRUSR | S_IWUSR | S_IRWXG | S_IRWXO;
    unsigned char *filename = malloc(sizeof(unsigned char) * 40);
    unsigned char *gettime = GetTime();
    unsigned char *tmp;    
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
    
    sprintf(filename, "%s%s.log", gettime, roomname);
    room->log_fd = open(filename, O_CREAT | O_WRONLY, mode);
    room->number = server->rooms->size + 1;

    if (room->is_secret) {
        tmp = ToHex(encrypt);
        DbCreateLog(server->db, filename, tmp);
        free(tmp);
    }
    free(gettime);
    free(filename);
    if (encrypt)
        free(encrypt);
    return room;
}

void PrintRoomList(Array *rooms, User *user) {
    int i, fd;
    Room *room;
    unsigned char *msg, *tmp;

    fd = user->fd;
    for (i = 0; i < rooms->size; i++) {
        room = rooms->data[i];
        if (room->is_secret) {
            tmp = MakeString(2, "*", room->name);
            printf("private room : %s\n", tmp);
            write(fd, tmp, strlen(tmp));
            free(tmp);
        } else {
            write(fd, room->name, strlen(room->name));
        }
        write(fd, " ", 1);
    }
    write(fd, "\n", 1);
    msg = "Room with * before of room name is private room.\nIf you want make private room, type the (room name)/(password)\nPlease input the room name : ";
    write(fd, msg, strlen(msg));
}

Room *FindRoomByName(Array *rooms, unsigned char *name) {
    int i;
    Room *room;

    for (i = 0; i < rooms->size; i++) {
        room = rooms->data[i];
        if (!strcmp(room->name, name)) return room;
    }
    return NULL;
}

Room *FindRoomByNumber(Array *rooms, int n) {
    int i;
    Room *room;
    
    for (i = 0; i < rooms->size; i++) {
        room = rooms->data[i];
        if (room->number == n) return room;
    }
    return NULL;
}

Room *MakeRoom(Server *server, unsigned char *buf) {
    int i;
    Room *room;
    unsigned char *name, *pass;
    
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
        name = substr(buf, 0, i);
        pass = substr(buf, i + 1, strlen(buf));

        if (!name || !pass) return NULL;
        room = InitRoom(server, name, pass);
        free(name);
        name = NULL;
        free(pass);
        pass = NULL;
    }

    return room;
}

void SendJoinMsg(Room *room, User *user) {    
    int i, *fd;
    unsigned char *msg = MakeString(3, "[", user->name, " is join the room]");
    unsigned char *encrypt;

    if (room->is_secret) {
        encrypt = Encrypt(room->aes, msg);
        
        for (i = 0; i < room->user_fds->size;i++) {
            fd = room->user_fds->data[i];
            write(*fd, encrypt, ((strlen(msg) / 16) + 1) * 16);
        }
        free(encrypt);
    } else {
        for (i = 0; i < room->user_fds->size;i++) {
            fd = room->user_fds->data[i];
            write(*fd, msg, strlen(msg));
        }
    }
    free(msg);
}

void SendLeaveMsg(Room *room, User *user) {    
    int i, *fd;
    unsigned char *msg = MakeString(3, "[", user->name, " is leave the room]");

    if (room->is_secret) {
        unsigned char *encrypt = Encrypt(room->aes, msg);

        for (i = 0; i < room->user_fds->size;i++) {
            fd = room->user_fds->data[i];
            if (*fd != user->fd) write(*fd, encrypt, ((strlen(msg) / 16) + 1) * 16);
        }
        free(encrypt);
    } else {
        for (i = 0; i < room->user_fds->size;i++) {
            fd = room->user_fds->data[i];
            if (*fd != user->fd) write(*fd, msg, strlen(msg));
            
        }
    }
    free(msg);
}

int JoinRoom(Server *server, User *user, unsigned char *buf) {
    int i, *fd;
    unsigned char *name, *pass;
    Room *room;

    printf("join room : %s\n", buf);
    for (i = 0; i < strlen(buf); i++) {
        if (buf[i] == '/') break;
    }
    if (i > 16) {
       return 1;
    }
    else if (i == strlen(buf)) {    
        name = substr(buf, 0, i);
        pass = substr(buf, i, strlen(buf));
    } else {
        name = substr(buf, 0, i);
        pass = substr(buf, i + 1, strlen(buf));
    }
    if (!name || !pass) return 1;

    room = FindRoomByName(server->rooms, name);
    
    if (!room) {
        // Make Room
        printf("Make room\n");
        room = InitRoom(server, name, pass);
        free(name);
        if (pass) free(pass);
        if (!room) return 1;
        user->room_number = room->number;
        fd = NewElement(room->user_fds);
        *fd = user->fd;
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
            SendJoinMsg(room, user);
            write(user->fd, "JOIN", 4);
            fd = NewElement(room->user_fds);
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
    int i, j, *fd;
    Room *room, *tmp;
    room = FindRoomByNumber(server->rooms, user->room_number);
    printf("Leave Room %s\n", room->name);
    SendLeaveMsg(room, user);
    for (i = 0; i < room->user_fds->size; i++) {
        fd = room->user_fds->data[i];
        if (*fd == user->fd) {
            printf("\tuser %d leave room number %ld\n", user->fd, room->number);
            EraseElement(room->user_fds, i);
            break;
        }
    }
    if (room->user_fds->size == 0) {
        for (i = 0; i < server->rooms->size; i++) {
            tmp = server->rooms->data[i];
            if (tmp->number == room->number) {
                printf("\tDelete %ld %s room\n", tmp->number, tmp->name);
                close(room->log_fd);
                if (room->aes) free(room->aes);
                FreeArray(room->user_fds);
                EraseElement(server->rooms, i);
                break;
            }
        }
    }
    user->status = LOBBY;
    
    return 0;
}

void RequestRoomPass(Room *room, User *user) {
    unsigned char *msg = "This is private room.\nPlease enter room password : ";
    
    write(user->fd, msg, strlen(msg));
    user->room_number = room->number;
    user->status = TRY_PRIVATE;
}

int TryPrivateRoom(Server *server, User *user) {
    Room *room = FindRoomByNumber(server->rooms, user->room_number);
    int *fds;

    if (!strcmp(user->buf, room->password)) {
        if (!(fds = NewElement(room->user_fds))) {
            user->status = LOBBY;
            printf("insert to user_fds failed\n");
            return false;
        }
        user->status = PRIVATE;
        SendJoinMsg(room, user);
        *fds = user->fd;
        write(user->fd, room->aes->key, 16);
        return true;
    }
    user->status = LOBBY;
    
    return false;
}

void FreeRooms(Array *rooms) {
    int i;
    Room *room;

    for (i = 0; i < rooms->size; i++) {
        room = rooms->data[i];
        if (room->aes) free(room->aes);
        FreeArray(room->user_fds);
    }
    FreeArray(rooms);
}