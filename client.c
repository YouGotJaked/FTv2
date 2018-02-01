/* 
 * File:   client.c
 * Author: jakeday
 *
 * Created on February 1, 2018, 9:18 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.h"

#define SIZE 1024

int main(int argc, char **argv) {
    int sock, port, bytes;
    char buffer[SIZE];
    struct sockaddr_in serv_addr;
    socklen_t addr_size;
    
    if (argc != 3) {
        printf("Usage: %s <port number> <ip of server>\n", argv[0]);
        return -1;
    }
    
    //configure address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    
    if (inet_pton(AF_INET, argv[2], &serv_addr.sin_addr) <= 0) {
        perror("inet_pton error occured");
        return -1;
    }
    
    memset(serv_addr.sin_zero, '\0', sizeof(serv_addr.sin_zero));
    addr_size = sizeof(serv_addr);
    
    //create UDP socket for server
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Could not create socket");
        return -1;
    }
    
    while (1) {
        fgets(buffer, SIZE, stdin);
        bytes = strlen(buffer) + 1;
        
        printf("Sending: %s\n", buffer);
        sendto(sock, buffer, bytes, 0, (struct sockaddr *)&serv_addr, addr_size);
        
        bytes = recvfrom(sock, buffer, SIZE, 0, NULL, NULL);
        printf("Received: %s\n", buffer);
        
        close(sock);
        return 0;
    }
}
/*
int chksum(char *buffer, size_t buff_size) {
    char *p,*cs;
    p = (char *)buffer;
    cs = p++;
    
    for (int i = 0; i < buff_size; i++) {
        cs = cs ^ *p;
        cs++;
        p++;
    }
    
    return (int)cs;
}
*/
