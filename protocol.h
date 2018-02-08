#ifndef PROTOCOL_H
#define PROTOCOL_H

struct header {
    int seq_ack;
    int len;
    int chksum;
}Header;

struct packet {
    struct header head;
    char data[10];
}Packet;

int chksum(char *buffer, size_t buff_size);

#endif /* PROTOCOL_H */

// helpful: https://www.youtube.com/watch?v=tPzoRLCX-Ps
