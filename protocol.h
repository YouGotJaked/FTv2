#ifndef PROTOCOL_H
#define PROTOCOL_H

#define SIZE 10

typedef struct header {
    int seq_ack;
    int len;
    int chksum;
} Header;

typedef struct packet {
    struct header head;
    char data[SIZE];
} Packet;

int chksum(char *buffer, size_t buff_size);
int csum(char *buffer, size_t buff_size);

#endif /* PROTOCOL_H */

// helpful: https://www.youtube.com/watch?v=tPzoRLCX-Ps
