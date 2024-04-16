#include "../../incs/room.h"

char *GetTime() {
    time_t timer;
    struct tm* t;
    char *ret = malloc(sizeof(char) * 19);
    timer = time(NULL);
    t = localtime(&timer);

    sprintf(ret, "%02d-%02d-%02d %02d:%02d:%02d ", t->tm_year - 100, t->tm_mon + 1,
                                                   t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    return ret;
}

Room *InitRoom(Server *server, char *roomname, char *password) {
    Room *room = malloc(sizeof(Room));
    char *filename = malloc(sizeof(char) * 40);
    
    room->number = server->rooms->size;
    strcpy(room->name, roomname);
    // if (!password) {
    if (strlen(password) == 0) {
        room->is_secret = false;
    }
    else {
        room->is_secret = true;
        strcpy(room->password, password);
    }

    room->user_fds = InitArray(sizeof(int));
    printf("\t Init Room\n");
    printf("datasize : %d\tsize: %d\n", room->user_fds->data_size, room->user_fds->size);
    
    char *gettime = GetTime();
    sprintf(filename, "%s%s.log", gettime, roomname);
    room->log_fd = fopen(filename, "w");
    InsertArray(server->rooms, (void *)room);

    free(gettime);
    free(filename);
    return room;
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

Room *MakeRoom(Server *server, char *name) {
    printf("Make room\n");
    Room *room = InitRoom(server, name, "");
    return room;
}

int JoinRoom(Server *server, User *user, char *name) {
    Room *room = FindRoomByName(server->rooms, name);
    
    if (room == NULL) {
        room = MakeRoom(server, name);
    } else {
        printf("Join room\n");
        if (room->is_secret) {
            RequestRoomPass(room, user);
            return 1;
        }
    }
    InsertArray(room->user_fds, &user->fd);
    user->status = PUBLIC;
    user->room_number = room->number;
    printf("\tuser %d join room number %d\n", user->fd, user->room_number);
}

int LeaveRoom(Server *server, User *user) {
    Room *room = FindRoomByNumber(server, user->room_number);
}

int RequestRoomPass(Room *room, User *user) {
    char *msg = "This is private room.\nPlease enter room password : ";
    write(user->fd, msg, strlen(msg));
    user->status = TRY_PRIVATE;
}