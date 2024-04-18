#include "../../incs/command.h"

int SetId(Server *server, User *user, char *buf) {
    printf("SetId : %s\n", buf);
    memcpy(user->name, buf, strlen(buf));
    printf("username : %s\n", user->name);
    char *msg;

    if (DbGetUser(server->db, buf)) {
        // Login
        printf("found user\n");
        user->status = WAIT_PASS;
        msg = "Please enter your password : ";
        int n = write(user->fd, msg, strlen(msg));
    } else {
        // create user
        printf("not found user\n");
        user->status = WAIT_REGIST;
        msg = "Not exist user\nCreate the account\nPlease enter your password : ";
        int n = write(user->fd, msg, strlen(msg));
    }
}

void Login(Server *server, User *user) {
    char msg[32];
    
    sprintf(msg, "Welcome %s!\n", user->name);
    write(user->fd, msg, strlen(msg));
    PrintRoomList(server->rooms, user);
}

int CreateUser(Server *server, User *user, char *buf) {
    uint8_t *pw = Encrypt(server->aes, (uint8_t *)buf);
    char *pw_hex = ToHex(pw);
    int ret = DbCreateUser(server->db, user->name, pw_hex);
    free(pw);
    free(pw_hex);
    Login(server, user);

    return ret;
}

bool TryLogin(Server *server, User *user, char *buf) {
    printf("TryLogin : %s\n", buf);
    char *saved_pw = DbGetUser(server->db, user->name);
    char *pw_str = ToString(saved_pw);
    char *decrypt_db = Decrypt(server->aes, pw_str, LOGIN_WORD_LIMIT);
    bool ret = false;
    char msg[32];
    if (!strcmp(buf, decrypt_db)) {
        Login(server, user);
        ret = true;
    }
    else {
        sprintf(msg, "Login Failed: Wrong password\n");
        write(user->fd, msg, strlen(msg));
    }
    free(pw_str);
    free(decrypt_db);
    return ret;
}

int Lobby(Server *server, User *user, char *buf) {

}
