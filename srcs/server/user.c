#include "../incs/user.h"

User *NewUser(int fd) {
    printf("new user : %d\n", fd);
    struct linger _linger;
    User *user = (User *)malloc(sizeof(User));
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);

    _linger.l_onoff = 1;
    _linger.l_linger = 0;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, &_linger, sizeof(_linger));    
    memset(buf, 0, sizeof(char) * BUF_SIZE);

    user->room_number = -1;
    user->fd = fd;
    user->buf = buf;
    user->buf_len = 0;
    user->status = WAIT_ID;

    return user;
}

User *UserByFd(Array *users, int fd) {
    int i;

    for (i = 0; i < users->size; i++) {
        User *user = users->data[i];
        if (user->fd == fd) return user;
    }

    printf("return user NULL\n");
    return NULL;
}

void DeleteUserByFd(Array *users, int fd) {
    int i;

    for (i = 0; i < users->size; i++) {
        User *user = users->data[i];
        if (user->fd == fd) {
            EraseArray(users, i);
            break;
        }
    }
}