/* 
 * File:   rijandael.c
 * Author: Marco Heumann
 *
 * Created on 29. Januar 2016
 */

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char u8;

int t2(int);
u8 getSboxValue(int);
void addRoundKey(int);
void generateRoundKey();
void shiftRow();
void mixColumn();

static const u8 Sbox[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};
/* Round constants */
static const u8 Rcon[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};
u8 mixMatrix[4][4] = {
    {0x02, 0x03, 0x01, 0x01},
    {0x01, 0x02, 0x03, 0x01},
    {0x01, 0x01, 0x02, 0x03},
    {0x03, 0x01, 0x01, 0x02}
};
u8 tmp[4] = {0, 0, 0, 0};

u8 key[4][4], roundKey[11][4][4], state[4][4];
int i, j, k, r, z;

void convert2array(u8 input[16], u8 output[4][4]) {
    for (j = 0; j < 4; j++) {
        for (i = 0; i < 4; i++) {
            output[i][j] = input[(j * 4 + i)];
        }
    }
}

void encrypt(u8 input[16], u8 keyStr[16], u8 output[16]) {
    convert2array(input, state);
    convert2array(keyStr, key);

    // Round 0
    generateRoundKey();
    addRoundKey(0);

    // Round 1-9
    for (r = 1; r < 10; r++) {
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                state[i][j] = getSboxValue(state[i][j]);
            }
        }
        shiftRow();
        mixColumn();
        addRoundKey(r);
    }

    // Round 10
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            state[i][j] = getSboxValue(state[i][j]);
        }
    }
    shiftRow();
    addRoundKey(r);

    for (j = 0; j < 4; j++) {
        for (i = 0; i < 4; i++) {
            output[(j * 4 + i)] = state[i][j];
        }
    }
}

u8 getSboxValue(int num) {
    return Sbox[num];
}

u8 getRconValue(int num) {
    return Rcon[num];
}

void shiftRow() {
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmp[j] = state[i][j];
        }
        for (j = 0; j < 4; j++) {
            state[i][j] = tmp[(j + i) % 4];
        }
    }
}

void mixColumn() {
    for (j = 0; j < 4; j++) {
        for (i = 0; i < 4; i++) {
            tmp[i] = state[i][j];
        }
        for (i = 0; i < 4; i++) {
            for (z = 0; z < 4; z++) {
                switch (mixMatrix[i][z]) {
                    case 3:
                        if (z == 0) {
                            state[i][j] = t2(tmp[z]) ^ tmp[z];
                        } else {
                            state[i][j] ^= t2(tmp[z]) ^ tmp[z];
                        }
                        break;
                    case 2:
                        if (z == 0) {
                            state[i][j] = t2(tmp[z]);
                        } else {
                            state[i][j] ^= t2(tmp[z]);
                        }
                        break;
                    case 1:
                    default:
                        if (z == 0) {
                            state[i][j] = tmp[z];
                        } else {
                            state[i][j] ^= tmp[z];
                        }
                }
            }
        }
    }
}

void addRoundKey(int round) {
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            state[i][j] ^= roundKey[round][i][j];
        }
    }
}

void generateRoundKey() {
    for (j = 0; j < 4; j++) {
        for (i = 0; i < 4; i++) {
            roundKey[0][i][j] = key[i][j];
        }
    }
    for (k = 1; k < 11; k++) {
        tmp[0] = roundKey[k - 1][0][3];

        // RotWord
        for (i = 0; i < 4; i++) {
            if (i < 3) {
                roundKey[k][i][0] = roundKey[k - 1][i + 1][3];
            } else {
                roundKey[k][i][0] = tmp[0];
            }
        }

        for (i = 0; i < 4; i++) {
            // Substitute with Sbox
            roundKey[k][i][0] = getSboxValue(roundKey[k][i][0]);

            // XOR Rcon
            if (i == 0) {
                roundKey[k][i][0] ^= roundKey[k - 1][i][0] ^ getRconValue(k - 1);
            } else {
                roundKey[k][i][0] ^= roundKey[k - 1][i][0];
            }
        }

        // XOR Rest
        for (j = 1; j < 4; j++) {
            for (i = 0; i < 4; i++) {
                roundKey[k][i][j] = roundKey[k - 1][i][j] ^ roundKey[k][i][j - 1];
            }
        }
    }
}

int t2(int a) {
    if (a < 128) {
        return 2 * a;
    } else if (a >= 128) {
        return (2 * a) ^ 283;
    }
}