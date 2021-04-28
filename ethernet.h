#ifndef _ETHERNET_H_
#define _ETHERNET_H_

#define MAX_ETHERNET_FRAME_SIZE 1514
#define MAX_ETHERNET_DATA_SIZE 1500

#define ETHERNET_HEADER_SIZE 14

#define MAC_BYTES 6

/**
 * struct for an ethernet frame
 **/
struct ethernet_frame {
    // destination MAC address, 6 bytes
    unsigned char dst_addr[6];

    // source MAC address, 6 bytes
    unsigned char src_addr[6];

    // type, in network byte order
    unsigned short type;

    // data
    unsigned char data[MAX_ETHERNET_DATA_SIZE];
};

int createEtherSocket(const char* iface, short proto);
int getLocalMacAddr(const char* iface, unsigned char* mac);
#if 0
char* getMacFromArp(const char* req_ip);
#else
int getMacFromArp(const char* req_ip, char* macStr);
#endif
int macAton(const char* a, unsigned char *n);
int sendEtherData(int sfd, unsigned char* to, unsigned char* from, short type, const char* data, int data_size);

#endif