// #define AES128 1
// #define AES192 1
// #define AES256 1

#include "aes.h"

uint8_t Iv[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
// uint8_t KEY[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };

static uint8_t xtime(uint8_t x)
{
  return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}

static uint8_t Multiply(uint8_t x, uint8_t y)
{
  return (((y & 1) * x) ^
       ((y>>1 & 1) * xtime(x)) ^
       ((y>>2 & 1) * xtime(xtime(x))) ^
       ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^
       ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))));
}

Aes *AesInit(uint8_t *key) {
    int i;
    Aes *aes = malloc(sizeof(Aes));

    memcpy(aes->iv, Iv, 16);
    aes->rcon[0] = 0x8d;
    memcpy(aes->key, key, 4 * NK);
    for (i = 1; i <= NR; i++) {
        uint16_t tmp = aes->rcon[i - 1];
        if (tmp >= 0x80) aes->rcon[i] = tmp * 2 ^ 0x11B;
        else aes->rcon[i] = tmp * 2;
    }
    KeyExpansion(aes);
    
    return aes;
}


void print_state(uint8_t (*state)[4]) {
    printf("state\n");
    for (int j = 0; j < 4; j++) {
        for (int k = 0; k < 4; k++) {
            printf("%02X  ", state[j][k]);
        }
    }
    printf("\n");
}

void KeyExpansion(Aes *aes) {
    int i, j;

    for (i = 0; i < NK; i++) {
        // Split Key
        aes->round_keys[i][0] = aes->key[i * 4 + 0];
        aes->round_keys[i][1] = aes->key[i * 4 + 1];
        aes->round_keys[i][2] = aes->key[i * 4 + 2];
        aes->round_keys[i][3] = aes->key[i * 4 + 3];
    }

    for (i = NK; i < _ROUND_LEN; i++) {
        uint8_t tmpa[4];
        for (j = 0; j < 4; j++) {
            tmpa[j] = aes->round_keys[i - 1][j];
        }
        // RotWord
        if (i % NK == 0) {
            uint8_t swp = tmpa[0];
            for (j = 0; j < NB - 1; j++) {
                tmpa[j] = tmpa[j + 1];
            }
            tmpa[NB - 1] = swp;

            // SubWord
            for (j = 0; j < NB; j++) {
                tmpa[j] = sbox[tmpa[j]];
            }

            tmpa[0] = tmpa[0] ^ aes->rcon[i / NK];
        }

        for (j = 0; j < NB; j++) {
            aes->round_keys[i][j] = aes->round_keys[i - NK][j] ^ tmpa[j];
        }
    }
}

void AddRoundKey(uint8_t round, uint8_t (*state)[4], uint8_t (*round_keys)[4]) {
    uint8_t i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            state[i][j] ^= round_keys[NB * round + i][j];
        }
    }
}

void SubBytes(uint8_t (*state)[4]) {
    uint8_t i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            state[i][j] = sbox[state[i][j]];
        }
    }
}

void InvSubBytes(uint8_t (*state)[4]) {
    uint8_t i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            state[i][j] = rsbox[state[i][j]];
        }
    }
}


void ShiftRows(uint8_t (*state)[4]) {
    uint8_t tmp;

    tmp = state[0][1];
    state[0][1] = state[1][1];
    state[1][1] = state[2][1];
    state[2][1] = state[3][1];
    state[3][1] = tmp;

    tmp = state[0][2];
    state[0][2] = state[2][2];
    state[2][2] = tmp;
    tmp = state[1][2];
    state[1][2] = state[3][2];
    state[3][2] = tmp;

    tmp = state[0][3];
    state[0][3] = state[3][3];
    state[3][3] = state[2][3];
    state[2][3] = state[1][3];
    state[1][3] = tmp;
}

void InvShiftRows(uint8_t (*state)[4]) {
    uint8_t tmp;

    tmp = state[3][1];
    state[3][1] = state[2][1];
    state[2][1] = state[1][1];
    state[1][1] = state[0][1];
    state[0][1] = tmp;

    tmp = state[0][2];
    state[0][2] = state[2][2];
    state[2][2] = tmp;
    tmp = state[1][2];
    state[1][2] = state[3][2];
    state[3][2] = tmp;

    tmp = state[0][3];
    state[0][3] = state[1][3];
    state[1][3] = state[2][3];
    state[2][3] = state[3][3];
    state[3][3] = tmp;
}

void MixColumns(uint8_t (*state)[4]) {
    uint8_t i, j, tmp, mandatory, res[4];
    uint8_t Tmp, Tm, t;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            res[j] = state[i][j] ^ state[i][j + 1];
            res[j] = ((res[j] << 1) ^ (((res[j] >> 7) & 1) * 0x1b));
        }
        res[3] = state[i][3] ^ state[i][0];
        res[3] = ((res[3] << 1) ^ (((res[3] >> 7) & 1) * 0x1b));

        mandatory = state[i][0] ^ state[i][1] ^ state[i][2] ^ state[i][3];
        
        state[i][0] ^= mandatory ^ res[0];
        state[i][1] ^= mandatory ^ res[1];
        state[i][2] ^= mandatory ^ res[2];
        state[i][3] ^= mandatory ^ res[3];
    }
}

void InvMixColumns(uint8_t (*state)[4]) {
    uint8_t tmp[4], i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmp[j] = state[i][j];
        }

        state[i][0] = Multiply(tmp[0], 0x0e) ^ Multiply(tmp[1], 0x0b) ^ Multiply(tmp[2], 0x0d) ^ Multiply(tmp[3], 0x09);
        state[i][1] = Multiply(tmp[0], 0x09) ^ Multiply(tmp[1], 0x0e) ^ Multiply(tmp[2], 0x0b) ^ Multiply(tmp[3], 0x0d);
        state[i][2] = Multiply(tmp[0], 0x0d) ^ Multiply(tmp[1], 0x09) ^ Multiply(tmp[2], 0x0e) ^ Multiply(tmp[3], 0x0b);
        state[i][3] = Multiply(tmp[0], 0x0b) ^ Multiply(tmp[1], 0x0d) ^ Multiply(tmp[2], 0x09) ^ Multiply(tmp[3], 0x0e);
    }
}

void Cipher(uint8_t (*state)[4], uint8_t (*round_keys)[4]) {
    uint8_t round = 0;

    AddRoundKey(0, state, round_keys);

    for (round = 1; ; round++) {
        SubBytes(state);
        ShiftRows(state);
        if (round == NR) break;
        MixColumns(state);
        AddRoundKey(round, state, round_keys);
    }
    AddRoundKey(round, state, round_keys);
}

void InvCipher(uint8_t (*state)[4], uint8_t (*round_keys)[4]) {
    uint8_t round = 0;

    AddRoundKey(NR, state, round_keys);

    for (round = NR - 1; ; --round) {
        InvShiftRows(state);
        InvSubBytes(state);
        AddRoundKey(round, state, round_keys);
        if (round == 0) break;
        InvMixColumns(state);
    }
}

void XorIV(uint8_t *buf, uint8_t *iv) {
    uint8_t i;

    for (i = 0; i < _BLOCK_LEN; i++) {
        buf[i] ^= iv[i];
    }
}


char *Encrypt(Aes *aes, char *buf) {
    int len = strlen(buf);
    int size = len % 16 == 0 ? len : ((len / 16) + 1) * 16;
    char *cipher = malloc(sizeof(char) * size + 1);
    char *ret = cipher;
    int i;
    memcpy(aes->iv, Iv, 16);
    memcpy(cipher, buf, len);
    for (i = len; i < size; i++) {
        cipher[i] = size - len;
    }
    for (i = 0; i < size; i += _BLOCK_LEN) {
        XorIV(cipher, aes->iv);
        Cipher((uint8_t (*)[4])cipher, aes->round_keys);
        memcpy(aes->iv, cipher, 16);
        cipher += _BLOCK_LEN;
    }
    *cipher = '\0';
    return ret;
}

char *Decrypt(Aes *aes, char *buf, int size) {
    int i;
    uint8_t next_iv[_BLOCK_LEN];
    char *buf_ptr = buf;
    char *plain = malloc(sizeof(char) * size + 1);
    char *ret = plain;

    memcpy(aes->iv, Iv, 16);
    for (i = 0; i < size; i += _BLOCK_LEN) {
        memcpy(next_iv, buf_ptr, 16);
        InvCipher((uint8_t (*)[4])buf_ptr, aes->round_keys);
        XorIV(buf_ptr, aes->iv);
        memcpy(&plain[i], buf_ptr, 16);
        memcpy(aes->iv, next_iv, 16);
        buf_ptr += _BLOCK_LEN;
    }
    plain[size] = '\0';
    if (plain[size - 1] <= 0x0F) {
        for (i = size - plain[size - 1]; i < size; i++) {
            plain[i] = 0;
        }
    }
    return ret;
}
