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
#include "csum.c"

extern int csum(char *buffer, size_t buff_size);

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
    struct sockaddr_in serv_addr;
    socklen_t addr_size;
    Packet p_client, p_server;
    p_client.head.chksum = 0;
    
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
    strcpy(p_server.data, argv[4]);
    sendto(sockfd, (char*)&p_server, sizeof(p_server), 0, (struct sockaddr *)&serv_addr, addr_size);
    printf("Sent filename: [%s]\n",p_server.data);
    
    //initialize file
    FILE *fr;
    fr = fopen(argv[3], "r");
    if (fr == NULL) {
        printf("File %s not found.\n", argv[3]);
        return -1;
    }
    
    do {
        //send data
        while ((bytes = fread(p_client.data, 1, sizeof(p_client.data), fr)) != 0) {
            printf(" data: [%s], bytes: [%d]", p_client.data,bytes);
            //p_client.head.len += sendto(sockfd, p_client.data, bytes, 0, (struct sockaddr *)&serv_addr, addr_size);
            p_client.head.len += sendto(sockfd, (char*)&p_client, bytes, 0, (struct sockaddr *)&serv_addr, addr_size);
        }
        printf("Sent data: [%s]\n",p_client.data);
        if (p_client.head.len > 0) {
            printf("[+] Packet sent.\n");
            printf("Client len: %d\n", p_client.head.len);
            p_client.head.seq_ack = 1;
        } else {
            perror("[-] Packet not sent.\n");
            p_client.head.seq_ack = 0;
        }
    
        //compute and send checksum
        p_client.head.chksum = chksum((char*)&p_client, sizeof(p_client));
        //sendto(sockfd, &p_client.head.chksum, sizeof(int), 0, (struct sockaddr *)&serv_addr, addr_size);
        sendto(sockfd, (char*)&p_client, sizeof(p_client), 0, (struct sockaddr *)&serv_addr, addr_size);
        printf("Client checksum: %d\n",p_client.head.chksum);
    
        //receive length and checksum from server
        recvfrom(sockfd, &p_server.head.len, sizeof(int), 0, (struct sockaddr *)&serv_addr, &addr_size);
        printf("Server len: %d\n",p_server.head.len);
    
        recvfrom(sockfd, &p_server.head.chksum, sizeof(int), 0, (struct sockaddr *)&serv_addr, &addr_size);
        printf("Server checksum: %d\n",p_server.head.chksum);
        
        p_server.head.seq_ack = p_server.head.len > 0 ? 1 : 0;
        if (p_server.head.len > 0) {
            printf("[+] ACK received.\n");
        } else {
            perror("[-] ACK not received.\n");
        }
    } while (p_client.head.chksum != p_server.head.chksum || p_client.head.seq_ack != p_server.head.seq_ack);
    
    close(sockfd);
    return 0;
}

