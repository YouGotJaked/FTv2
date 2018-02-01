#ifndef PROTOCOL_H
#define PROTOCOL_H

struct header {
    int seq_ack;
    int len;
    int cksum;
};

struct packet {
    struct header head;
    char data[10];
};

int chksum(char *buffer, size_t buff_size);

#endif /* PROTOCOL_H */
