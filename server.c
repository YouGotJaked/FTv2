/* 
 * File:   server.c
 * Author: jakeday
 *
 * Created on January 29, 2018, 3:16 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "protocol.h"

#include <ctype.h>

#define SIZE 1024

/*
 *  create UDP socket
 *  bind socket to specified port no
 *  loop forever
 *  read from UDP socket into message, getting client's IP and port no
 *  send string back to client
 */

int main(int argc, char **argv) {
    int listenfd = 0, connfd = 0, bytes;
    char buffer[SIZE];
    struct sockaddr_in serv_addr, client_addr;
    struct sockaddr_storage serv_storage;
    socklen_t addr_size, client_addr_size;
    
    Packet p_send, p_recv;
    
    if (argc != 2) {
        printf("Usage: %s <port number>\n", argv[0]);
        return -1;
    }
    
    //initialize
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //zero server
    memset((char *)serv_addr.sin_zero, '\0', sizeof(serv_addr.sin_zero));
    addr_size = sizeof(serv_storage);
    
    //create UDP socket
    if ((listenfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket error");
        return -1;
    }
    
    //bind socket
    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
        perror("Bind error");
        return -1;
    }
    
    printf("Press 'Q' and hit return to exit server.\n");
    
    //loop forever
    while (getchar() != 'Q') {
        bytes = recvfrom(connfd, buffer, SIZE, 0, (struct sockaddr *)&serv_storage, &addr_size);
        
        if (bytes > 0) {
            printf("[+] Packet received: %s\n", p_recv.data);
        }
        
        for (int i = 0; i < bytes - 1; i++) {
            buffer[i] = tolower(buffer[i]);
        }
        
        sendto(sock, buffer, bytes, 0, (struct sockaddr *)&serv_storage, addr_size);
    }
    
    close(listenfd);
    close(connfd);
    return 0;
}

