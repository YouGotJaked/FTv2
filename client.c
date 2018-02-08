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

/*
 *  include socket library
 *  create UDP socket for server
 *  get user keyboard input
 *  attach server name, port to message; send into socket
 *  read reply char from socket into string
 *  print out received string and close socket
 */

int main(int argc, char **argv) {
    int sockfd, port, bytes;
    char buffer[SIZE];
    struct sockaddr_in serv_addr;
    socklen_t addr_size;
    
    Packet p_send, p_recv;
    int ack_recv = 1;
    
    if (argc != 5) {
        printf("Usage: %s <port number> <ip of server> <source file> <destination file>\n", argv[0]);
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
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Could not create socket");
        return -1;
    }
    
    //send output file name to server
    write(sockfd, argv[4], strlen(argv[4]) + 1);
    
    //send file to server
    FILE *fr;
    fr = fopen(argv[3], "r");
    if (fr == NULL) {
        printf("File %s not found.\n", argv[3]);
        return -1;
    }
    
    do {
        //change to file not stdin
        fgets(buffer, SIZE, stdin);
        strcpy(p_send.data, buffer);
        bytes = strlen(buffer) + 1;
        
        p_send.head.chksum = chksum(buffer,SIZE);
        
        printf("[+] Packet sent.\n");
        sendto(sockfd, &p_send, sizeof(Packet), 0, (struct sockaddr *)&serv_addr, addr_size);
        
        bytes = recvfrom(sockfd, &p_recv, sizeof(Packet), 0, NULL, NULL);
        
        if (bytes > 0) {
            printf("[+] ACK received.\n");
            p_recv.head.seq_ack = 1;
        } else {
            printf("[-] ACK not received.\n");
            p_recv.head.seq_ack = 0;
        }

    } while (p_send.head.chksum != p_recv.head.chksum || p_send.head.seq_ack != p_recv.head.seq_ack);
    
    close(sockfd);
    return 0;
}

int chksum(char *buffer, size_t buff_size) {
    char *p,cs;
    p = (char *)buffer;
    cs = *p++;
    
    for (int i = 0; i < buff_size; i++) {
        cs ^= *p;
        cs++;
        p++;
    }
    
    return (int)cs;
}

int csum(char *buffer, size_t buff_size) {
    long p;
    
    for (p = 0; buff_size > 0; buff_size--) {
        p += *buffer++;
    }
    
    p = (p >> 16) + (p &0xffff);
    p += (sum >> 16);
    return (int)(~sum);
}

