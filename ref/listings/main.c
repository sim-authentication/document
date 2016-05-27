/* 
 * File:   main.c
 * Author: Marco Heumann
 *
 * Created on 29. Januar 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "milenage.h"

typedef unsigned char u8;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

u8 inputArr[16], outputArr[16], auts[14];
u8 mrand[16];
u8 keyArr[16] = {0x14, 0x5D, 0xD6, 0x1C, 0xE1, 0xF1, 0x9B, 0x84, 0x96, 0x3C, 0x09, 0x8C, 0x84, 0xDF, 0x1B, 0x98};
int i, status;
int socket_desc, client_sock, c, read_size, optval;
struct sockaddr_in server, client;
char client_message[50], imsi[15], response_message[96];

int main(int argc, char** argv) {
    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf(ANSI_COLOR_GREEN "Could not create socket" ANSI_COLOR_RESET);
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(12345);

    if (bind(socket_desc, (struct sockaddr *) &server, sizeof (server)) < 0) {
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    listen(socket_desc, 3);

    puts("Waiting for incoming connections...");
    c = sizeof (struct sockaddr_in);

    client_sock = accept(socket_desc, (struct sockaddr *) &client, (socklen_t*) & c);
    if (client_sock < 0) {
        perror(ANSI_COLOR_RED "accept failed" ANSI_COLOR_RESET);
        return 1;
    }
    puts(ANSI_COLOR_GREEN "Connection accepted" ANSI_COLOR_RESET);

    //Receive a message from client
    while ((read_size = recv(client_sock, client_message, 50, 0)) > 0) {
        char* temp;
        temp = malloc(2);

        strncpy(temp, &client_message[0], 1);
        status = strtol(temp, NULL, 10);

	printf(ANSI_COLOR_BLUE "\nReceived message ");
        if (status == 1) {
            printf("(Status[1] || IMSI[15] )");
        } else {
            printf("(Status[1] || IMSI[15] || AUTS[28] )");
        }
        printf(": \n" ANSI_COLOR_RESET);
	puts(client_message);

        strncpy(imsi, &client_message[1], 15);
        
        response_message[0] = 50; // 50h -> 2d
            
        printf(ANSI_COLOR_BLUE "\nCalculated values:" ANSI_COLOR_RESET);

        if (status == 3) {
            for (i = 8; i < 22; i++) {
                strncpy(temp, &client_message[i * 2], 2);
                auts[i-8] = strtol(temp, NULL, 16);
            }
            
            f5star(keyArr, auts);
            response_message[0] = 52; // 52h -> 4d
        }
        free(temp);

        genRand(mrand, &response_message[16]);
        f1(keyArr, mrand);
        f2_5(keyArr, &response_message[72]);
        genAutn(&response_message[48]);
        
        for(i = 0; i < 15; i++) {
            response_message[i+1] = imsi[i];
        }

        //Send the message back to client
        write(client_sock, response_message, strlen(response_message));
        
	printf(ANSI_COLOR_BLUE "\n\nSent message (Status[1] || IMSI[15] || RAND[32] || AUTN[32] ): \n" ANSI_COLOR_RESET);
	puts(response_message);
    }

    if (read_size == 0) {
        puts(ANSI_COLOR_YELLOW "\nClient disconnected" ANSI_COLOR_RESET);
        fflush(stdout);
    } else if (read_size == -1) {
        perror(ANSI_COLOR_RED "\nrecv failed" ANSI_COLOR_RESET);
    }

    printf("\n");
    return (EXIT_SUCCESS);
}
