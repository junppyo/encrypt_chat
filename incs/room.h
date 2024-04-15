#pragma once
#include "utils.h"

typedef struct room {
    uint8_t room_number;
    bool is_secret;
    uint8_t *roomname;
    char password[16];
    Array *user_fds;
    int log_fd;
    // int user_fds[16];
} Room;