#include "../incs/user.h"

User *NewUser(int fd) {
    struct linger _linger;
    User *user = (User *)malloc(sizeof(User));
    unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char) * BUF_SIZE);

    _linger.l_onoff = 1;
    _linger.l_linger = 0;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, &_linger, sizeof(_linger));    
    memset(buf, 0, sizeof(unsigned char) * BUF_SIZE);

    user->room_number = 0;
    user->fd = fd;
    user->buf = buf;
    user->buf_len = 0;
    user->status = WAIT_ID;

    return user;
}

User *UserByFd(Array *users, int fd) {
    int i;
    User *user;

    for (i = 0; i < users->size; i++) {
        user = users->data[i];
        if (user->fd == fd) return user;
    }

    printf("return user NULL\n");
    return NULL;
}

void DeleteUserByFd(Array *users, int fd) {
    int i;
    User *user;

    for (i = 0; i < users->size; i++) {
        user = users->data[i];
        if (user->fd == fd) {
            free(user->buf);
            EraseElement(users, i);
            break;
        }
    }
}

int DisconnectUser(Server *server, int fd) {
    User *user = UserByFd(server->users, fd);
    printf(" disconnect user room num : %ld\n", user->room_number);
    if (user->room_number != 0) {
        if (user->status == PRIVATE || user->status == PUBLIC) {
            LeaveRoom(server, user);
        }
    }
    DeleteUserByFd(server->users, fd);

    close(fd);
}

void FreeUsers(Array *users) {
    int i;
    User *user;
    
    for (i = 0; i < users->size; i++) {
        user = users->data[i];
        free(user->buf);
    }
    FreeArray(users);
}