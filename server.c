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
#include <ctype.h>
#include "protocol.h"
#include "csum.c"

/*
 *  create UDP socket
 *  bind socket to specified port no
 *  loop forever
 *  read from UDP socket into message, getting client's IP and port no
 *  send string back to client
 */

int main(int argc, char **argv) {
    int connfd = 0, bytes;
    struct sockaddr_in serv_addr, client_addr;
    struct sockaddr_storage serv_storage;
    socklen_t addr_size, client_addr_size;
    FILE *fp;
    Packet p_client, p_server;

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
    if ((connfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket error");
        return -1;
    }
    
    //bind socket
    if (bind(connfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
        perror("Bind error");
        return -1;
    }
    
    //loop forever
    while (1) {
        //initialize checksum to 0
        p_server.head.chksum = 0;
        
        //get file name from client
        recvfrom(connfd, (char*)&p_client, sizeof(p_client), 0, (struct sockaddr *)&serv_storage, &addr_size);
        printf("Received file name: [%s]\n",p_client.data);
        fp = fopen(p_client.data, "wb");
        if (fp == NULL) {
            return -1;
        }
	
        //read data from client
        do {
            bytes = recvfrom(connfd, (char*)&p_server, sizeof(p_server), 0, (struct sockaddr *)&serv_storage, &addr_size);
            printf(" data: [%s], bytes: [%d]", p_server.data,bytes);
            fwrite(p_server.data, sizeof(char), bytes, fp);
            p_server.head.len += bytes;
        } while (bytes > (SIZE - 1));
        
        if (p_server.head.len > 0) {
            printf("[+] Packet received: %s\n", p_server.data);
        }
        
        //save copy of checksum from client header
        //recvfrom(connfd, &p_client.head.chksum, sizeof(int), 0, (struct sockaddr *)&serv_storage, &addr_size);
        recvfrom(connfd, (char*)&p_client, sizeof(p_client), 0, (struct sockaddr *)&serv_storage, &addr_size);
        printf("Client checksum = %d\n",p_client.head.chksum);
        
        //send length and checksum;
        p_server.head.chksum = chksum((char*)&p_server, sizeof(p_server));
        printf("Server checksum = %d\n",p_server.head.chksum);
        sendto(connfd, &p_server.head.len, sizeof(int), 0, (struct sockaddr *)&serv_storage, addr_size);
        sendto(connfd, &p_server.head.chksum, sizeof(int), 0, (struct sockaddr *)&serv_storage, addr_size);
        
        fclose(fp);
        close(connfd);
        return 0;
    }
}

