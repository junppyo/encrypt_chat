#include "../incs/chat.h"

Room *GetRoom(Server *server, uint8_t n) {
    Array *rooms = server->rooms;
    int i;

    for (i = 0; i < rooms->size; i++) {
        Room *room = rooms->data[i];
        if (room->room_number == n) return room;
    }
    return NULL;
}

Room *MakeRoom(Server *server, char *buf) {
    Room *room = malloc(sizeof(Room));
    room->room_number = server->room_number++;
    if (strlen(buf) > 4) {
        room->is_secret = true;
        memcpy(room->password, &buf[4], strlen(buf) - 5);
    } else
        room->is_secret = false;
    room->user_fds = InitArray(sizeof(int));

    return room;
}

void ProcessCommand(Server *server, User* user) {
    char *buf = user->buf;
    
    if (!strncmp(buf, "JOIN ", 5)) {

    } else if (!strncmp(buf, "MAKE", 4)) {
        Room *room = MakeRoom(server, buf);
        InsertArray(room->user_fds, user->fd);
        InsertArray(server->rooms, room);
    } else {
        SendMsg(server, user);
    }
    memcpy(buf, 0, BUF_SIZE);
}

void SendMsg(Server *server, User *user) {
    if (user->room_number == 0) {
        printf("user %d is not join the room\n", user->fd);
        char *s = "You don't join the room yet\n";
        send(user->fd, s, strlen(s), 0);
        return ;
    }

    Room *room = GetRoom(server, user->room_number);



}

