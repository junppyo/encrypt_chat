#include "../../incs/command.h"

void SetId(Server *server, User *user, unsigned char *buf) {
    printf("SetId : %s\n", buf);
    strcpy(user->name, buf);
    printf("username : %s\n", user->name);
    unsigned char *msg;
    unsigned char *pass = DbGetUser(server->db, buf);
    if (pass) {
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
    free(pass);
}

void Login(Server *server, User *user) {
    unsigned char msg[32];
    
    sprintf(msg, "Welcome %s!\n", user->name);
    write(user->fd, msg, strlen(msg));
    PrintRoomList(server->rooms, user);
}

int CreateUser(Server *server, User *user, unsigned char *buf) {
    uint8_t *pw = Encrypt(server->aes, (uint8_t *)buf);
    unsigned char *pw_hex = ToHex(pw);
    int ret = DbCreateUser(server->db, user->name, pw_hex);
    free(pw);
    free(pw_hex);
    Login(server, user);

    return ret;
}

bool TryLogin(Server *server, User *user, unsigned char *buf) {
    unsigned char *saved_pw = DbGetUser(server->db, user->name);
    unsigned char *pw_str = ToString(saved_pw);
    unsigned char *decrypt_db = Decrypt(server->aes, pw_str, LOGIN_WORD_LIMIT);
    bool ret = false;
    unsigned char msg[32];
    if (!strcmp(buf, decrypt_db)) {
        Login(server, user);
        ret = true;
    }
    else {
        sprintf(msg, "Login Failed: Wrong password\n");
        write(user->fd, msg, strlen(msg));
    }
    free(saved_pw);
    free(pw_str);
    free(decrypt_db);
    return ret;
}

int Lobby(Server *server, User *user, unsigned char *buf) {

}
