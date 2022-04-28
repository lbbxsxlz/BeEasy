#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if_ether.h>

#define MAX_ETHERNET_FRAME_SIZE 1514
#define MAX_ETHERNET_DATA_SIZE  1500

#define ETHERNET_HEADER_SIZE 14

#define MAC_BYTES 6
#define TYPE      0x1024

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

int main(int argc, char *argv[]) 
{
    int sfd = -1;
    int ret = -1;
    short type, old_type = 0;
    struct ethernet_frame frame;

    /* 只处理TYPE的以太网包 */
    sfd = socket(PF_PACKET, SOCK_RAW, htons(TYPE));
    if (sfd < 0) {
        perror("Fail to create Ether Socket \n");
        exit(-1);
    }

    while(1) {
        ret = recvfrom(sfd, &frame, sizeof(frame), 0, NULL, NULL);
        if (ret < 0) {
            perror("Fail to recv ether data \n");
        }

        type = ntohs(frame.type);

        if (type == TYPE) {
            printf("%s \n", frame.data);
        }

        usleep(10000);
    }

    return 0;
}

