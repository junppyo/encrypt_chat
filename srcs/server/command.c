#include "../../incs/command.h"

int SetId(Server *server, User *user, char *buf) {
    memcpy(user->name, buf, strlen(buf));
    printf("username : %s\n", user->name);
    char *msg;

    if (FindUser(server->db, buf)) {
        // Login
        printf("found user\n");
        user->status = WAIT_PASS;
        msg = "Please enter your password\0";
        printf("will send: %s\n", msg);
        int n = write(user->fd, msg, strlen(msg) + 1);
        printf("\tsend %d %s", n, msg);
        // send(user->fd, buf, strlen(msg), 0);
    } else {
        // create user
        printf("not found user\n");
        user->status = WAIT_REGIST;
        msg = "Not exist user\nCreate the account\nPlease enter your password\n";
        // write(user->fd, msg, strlen(msg));
        // send(user->fd, buf, strlen(msg), 0);
        int n = write(user->fd, msg, strlen(msg));
        printf("\tsend %d %s", n, msg);
    }
}

int CreateUser(Server *server, User *user, char *buf) {
    
}

int LoginUser(Server *server, User *user, char *buf) {

}

int Lobby(Server *server, User *user, char *buf) {

}