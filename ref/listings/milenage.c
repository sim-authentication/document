/* 
 * File:   milenage.c
 * Author: Marco Heumann
 *
 * Created on 05. Maerz 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rijndael.h"

typedef unsigned char u8;

void f1(u8*, u8*);
void convertToBin(u8*, u8*);
void convertToHex(u8*, u8*);
void genRand(u8*, u8*);
void genAutn(u8*);
void rotWord(u8*, int, int);
void convertToBin(u8*, u8*);

u8 opc[16] = {0x25, 0x50, 0x07, 0x6C, 0x5E, 0xF5, 0x9F, 0x77, 0xEE, 0xBE, 0xF1, 0xCA, 0x39, 0x91, 0x6E, 0xC8};
u8 c1[16] = {0x7C, 0x1E, 0xE3, 0x6E, 0x98, 0xC2, 0x74, 0x40, 0xCA, 0x1D, 0x58, 0xF7, 0xE8, 0xD3, 0x7D, 0x2F};
u8 c2[16] = {0xC1, 0xFC, 0xBC, 0xAB, 0xC7, 0x3E, 0xE4, 0xA2, 0xF3, 0x62, 0x82, 0x11, 0xB8, 0x7A, 0xED, 0x04};
u8 c3[16] = {0x16, 0x07, 0x87, 0xD0, 0x24, 0xC2, 0xA5, 0x1D, 0xC4, 0x21, 0x4A, 0x3A, 0xE4, 0x3F, 0x5A, 0x34};
u8 c4[16] = {0x69, 0xA6, 0xF7, 0x01, 0x54, 0x77, 0x69, 0x17, 0x85, 0xD8, 0x0C, 0xA1, 0xBB, 0x7A, 0x0D, 0x93};
u8 c5[16] = {0xFD, 0x2E, 0xF6, 0x92, 0xE4, 0x14, 0xFB, 0x2E, 0x93, 0xE3, 0x9C, 0x0A, 0x92, 0xD0, 0xE9, 0xAB};
u8 sqn[6] = {0xff, 0x9b, 0xb4, 0xd0, 0xb6, 0x07};
u8 amf[2] = {0xb9, 0xb9};
u8 binArr[128], temp[16], res[8], ak[6], in1[16], toEncrypt[16], out1[16];

int i;

void f1(u8* keyArr, u8* mrand) {
    printf("\nSQN: ");
    for (i = 0; i < 6; i++) {
        printf("%02x", sqn[i]);
    }
    
    printf("\nAMF: ");
    for (i = 0; i < 2; i++) {
        printf("%02x", amf[i]);
    }

    // create IN1
    for (i = 0; i < 6; i++) {
        in1[i] = sqn[i];
        in1[i + 8] = sqn[i];
    }
    for (i = 0; i < 2; i++) {
        in1[i + 6] = amf[i];
        in1[i + 14] = amf[i];
    }

    printf("\nRAND: ");
    for (i = 0; i < 16; i++) {
        printf("%02x", mrand[i]);
    }

    for (i = 0; i < 16; i++) {
        toEncrypt[i] = mrand[i] ^ opc[i];
    }
    encrypt(toEncrypt, keyArr, temp);
    
    for (i = 0; i < 16; i++) {
        out1[i] = in1[i]^opc[i];
    }

    convertToBin(out1, binArr);
    rotWord(binArr, 128, 0x1F);
    convertToHex(binArr, out1);

    for (i = 0; i < 16; i++) {
        out1[i] ^= temp[i]^c1[i];
    }
    encrypt(out1, keyArr, out1);

    for (i = 0; i < 16; i++) {
        out1[i] ^= opc[i];
    }

    printf("\nMAC-A: ");
    for (i = 0; i < 8; i++) {
        printf("%02x", out1[i]);
    }

    printf("\nMAC-S: ");
    for (i = 8; i < 16; i++) {
        printf("%02x", out1[i]);
    }
}

void f2_5(u8* keyArr, u8* response_arr) {
    u8* out2;
    out2 = malloc(16);

    for (i = 0; i < 16; i++) {
        out2[i] = temp[i] ^ opc[i];
    }
    
    convertToBin(out2, binArr);
    rotWord(binArr, 128, 0x3A);
    convertToHex(binArr, out2);

    for (i = 0; i < 16; i++) {
        out2[i] ^= c2[i];
    }
    encrypt(out2, keyArr, out2);

    for (i = 0; i < 16; i++) {
        out2[i] ^= opc[i];
    }

    printf("\r\nAK: ");
    for (i = 0; i < 6; i++) {
        ak[i] = out2[i];
        printf("%02x", ak[i]);
    }
    printf("\r\nRES: ");
    for (i = 8; i < 16; i++) {
        sprintf(&response_arr[(i-8)*2], "%02x", out2[i]);
        printf("%02x", out2[i]);
    }
}

void f5star(u8* keyArr, u8* sqn_ak) {
    u8* out5;
    out5 = malloc(16);

    for (i = 0; i < 16; i++) {
        out5[i] = temp[i] ^ opc[i];
    }
    
    convertToBin(out5, binArr);
    rotWord(binArr, 128, 0x08);
    convertToHex(binArr, out5);

    for (i = 0; i < 16; i++) {
        out5[i] ^= c5[i];
    }
    encrypt(out5, keyArr, out5);

    for (i = 0; i < 16; i++) {
        out5[i] ^= opc[i];
    }

    printf("\r\nAK (f5*): ");
    for (i = 0; i < 6; i++) {
        ak[i] = out5[i];
        printf("%02x", ak[i]);
    }
    
    for (i = 0; i < 6; i++) {
        sqn[i] = ak[i] ^ sqn_ak[i];
    }
    
    u8 ind = (sqn[5] & 0b00011111);
    u8 seq = (sqn[5] & 0b11100000);
    ind = (ind + 1) % 32;
    sqn[5] = 0;
    sqn[5] |= ind;
    
    seq += 0b00100000;
    if (seq == 0) {
        sqn[4] += 1;
    }
    sqn[5] |= seq;
}

void convertToBin(u8* hexArr, u8* binArr) {
    int i, j;
    unsigned char mask = 1;

    for (j = 0; j < 16; j++) {
        for (i = 0; i < 8; i++) {
            binArr[j * 8 + i] = (hexArr[j] & (mask << (7 - i))) != 0;
        }
    }
}

void convertToHex(u8* binArr, u8* hexArr) {
    int i, arrpos; //i is for calculating each 8 bit to hex

    for (arrpos = 0; arrpos < 16; arrpos++) {
        hexArr[arrpos] = 0;
        for (i = 0; i < 8; i++) {
            hexArr[arrpos] <<= 1;
            hexArr[arrpos] |= binArr[arrpos*8 + i];
        }
    }
}

void genAutn(u8* response_arr) {
    printf("\nAUTN: ");
    for (i = 0; i < 6; i++) {
        sprintf(&response_arr[i*2], "%02x", (sqn[i] ^ ak[i]));
        printf("%02x", (sqn[i] ^ ak[i]));
    }
    for (i = 0; i < 2; i++) {
        sprintf(&response_arr[(i+6)*2], "%02x", amf[i]);
        printf("%02x", amf[i]);
    }
    for (i = 0; i < 8; i++) {
        sprintf(&response_arr[(i+8)*2], "%02x", out1[i]);
        printf("%02x", out1[i]);
    }
}

void genRand(u8* mrand, u8* response_arr) {
    srand(time(NULL));
    for (i = 0; i < 16; i++) {
        mrand[i] = (unsigned)rand() % 255;
        sprintf(&response_arr[i*2], "%02x", mrand[i]);
    }
}

void reverse(u8* a, int sz) {
    int i, j;
    for (i = 0, j = sz; i < j; i++, j--) {
        int tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}

void rotWord(u8* array, int size, int amt) {
    reverse(array, amt - 1);
    reverse(array + amt, size - amt - 1);
    reverse(array, size - 1);
}