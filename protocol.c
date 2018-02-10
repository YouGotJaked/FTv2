#include "protocol.h"

int chksum(char *buffer, size_t buff_size) {
    int i;
    char *p = (char*)p,cs;
    p = (char *)buffer;
    cs = *p++;
    
    for (i = 0; i < buff_size; i++) {
        cs ^= *p;
        cs++;
        p++;
    }
    
    return (int)cs;
}

int comp_packet(Packet p1, Packet p2) {
    return p1.head.len == p2.head.len && p1.head.chksum == p2.head.chksum;
}

int micros_to_s(int s) {
    return s / 1000000;
}
