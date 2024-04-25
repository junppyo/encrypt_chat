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

void SetId(Server *server, User *user, unsigned char *buf) {
    unsigned char *msg, *pass;

    printf("SetId : %s\n", buf);
    strcpy(user->name, buf);
    printf("username : %s\n", user->name);
    pass = DbGetUser(server->db, buf);
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
