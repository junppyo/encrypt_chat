#include "../incs/chat.h"

void SendMsg(Server *server, User *user) {
    int i, n, *fd;
    unsigned char *s;
    Room *room;

    if (user->room_number < 0) {
        printf("user %d is not join the room\n", user->fd);
        s = "You don't join the room yet\n";
        send(user->fd, s, strlen(s), 0);
        return ;
    }
    if (!strcmp(user->buf, "!exit")) {
        LeaveRoom(server, user);
        write(user->fd, "Leave\n", 6);
        PrintRoomList(server->rooms, user);
        return ;
    }
    room = FindRoomByNumber(server->rooms, user->room_number);
    write(room->log_fd, &user->buf_len, sizeof(unsigned char));
    n = write(room->log_fd, user->buf, user->buf_len);
    
    for (i = 0; i < room->user_fds->size; i++) {
        fd = room->user_fds->data[i];
        printf("%d  send user fd : %d       room user fd : %d\n", i, user->fd, *fd);
        if (*fd == user->fd) continue;
        write(*fd, user->buf, user->buf_len);
        printf("sendmsg %d : %s\n", *fd, user->buf);
    }    
}
