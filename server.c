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

int main(int argc, char **argv) {
    int sock, bytes;
    char buffer[SIZE];
    struct sockaddr_in serv_addr, client_addr;
    struct sockaddr_storage serv_storage;
    socklen_t addr_size, client_addr_size;
    
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
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket error");
        return -1;
    }
    
    //bind socket
    if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
        perror("Bind error");
        return -1;
    }
    
    //loop forever
    while (1) {
        bytes = recvfrom(sock, buffer, SIZE, 0, (struct sockaddr *)&serv_storage, &addr_size);
        
        for (int i = 0; i < bytes - 1; i++) {
            buffer[i] = tolower(buffer[i]);
        }
        
        sendto(sock, buffer, bytes, 0, (struct sockaddr *)&serv_storage, addr_size);
        
        close(sock);
        return 0;
    }
}

