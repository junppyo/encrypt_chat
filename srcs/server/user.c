#include "../incs/user.h"

User *NewUser(int fd) {
    printf("new user : %d\n", fd);
    User *user = (User *)malloc(sizeof(User));
    user->fd = fd;
    
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    memset(buf, 0, sizeof(char) * BUF_SIZE);
    user->buf = buf;
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