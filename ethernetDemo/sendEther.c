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

#define MAX_ETHERNET_FRAME_SIZE 1514
#define MAX_ETHERNET_DATA_SIZE  1500

#define ETHERNET_HEADER_SIZE 14

#define MAC_BYTES 6
#define TYPE      0x1024

char localMac[MAC_BYTES];

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


char* getMacFromArp(const char* req_ip)
{
    FILE *procFile;
    char ip[16];
    char mac[18];
    char * reply = NULL;
 
    if (!(procFile = fopen("/proc/net/arp", "r"))) {
        return NULL;
    }
 
    /* Skip first line */
    while (!feof(procFile) && fgetc(procFile) != '\n');
 
    /* Find ip, copy mac in reply */
    reply = NULL;
    while (!feof(procFile) && (fscanf(procFile, " %15[0-9.] %*s %*s %17[A-Fa-f0-9:] %*s %*s", ip, mac) == 2)) {
        if (strcmp(ip, req_ip) == 0) {
        reply = strdup(mac);
        break;
    }
    }
 
    fclose(procFile);
    return reply;
}


/**
 *  Convert readable MAC address to binary format.
 *
 *  Arguments
 *      a: buffer for readable format, like "08:00:27:c8:04:83".
 *
 *      n: buffer for binary format, 6 bytes at least.
 *
 *  Returns
 *      0 if success, -1 if error.
 **/
 
int macAton(const char* a, unsigned char *n) {
    int matches = sscanf(a, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", n, n+1, n+2,
                         n+3, n+4, n+5);

    return (6 == matches ? 0 : -1);
}

int getLocalNetInfo(const char* iface, int s, int* if_index, char* mac)
{
    // create socket if needed(fd is not given)
    int ret = -1;
    bool create_socket = (s < 0);
    if (create_socket) {
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (-1 == s) {
            return -1;
        }
    }

    // fill iface name to struct ifreq
    struct ifreq ifr;
    strncpy(ifr.ifr_name, iface, 15);

    // call ioctl system call to fetch iface index
    ret = ioctl(s, SIOCGIFINDEX, &ifr);
    if (-1 == ret) {
        goto cleanup;
    }

    *if_index = ifr.ifr_ifindex;

    ret = ioctl(s, SIOCGIFHWADDR, &ifr);
    if (-1 == ret) {
        goto cleanup;
    }

    memcpy(mac, ifr.ifr_hwaddr.sa_data, MAC_BYTES);

cleanup:
    // close socket if created here
    if (create_socket) {
        close(s);
    }

    return ret;
}

int createEtherSocket(const char* iface)
{
    int sfd = -1;
    int ret = -1;
    int if_index;
    // create socket if needed(s is not given)
    sfd = socket(PF_PACKET, SOCK_RAW | SOCK_CLOEXEC, 0);
    if (-1 == sfd) {
        return -1;
    }

    ret = getLocalNetInfo(iface, sfd, &if_index, localMac);
       
    struct sockaddr_ll sll;
    bzero(&sll, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_index;
    sll.sll_pkttype = PACKET_HOST;

    // call bind system call to bind socket with iface
    ret = bind(sfd, (struct sockaddr *)&sll, sizeof(sll));
    if (-1 == ret) {
        return -1;
    }

    return sfd;
}

int sendEtherData(int s, char* to, short type, const char* data)
{
    int ret = -1;
    // construct ethernet frame, which can be 1514 bytes at most
    struct ethernet_frame frame;

    // fill destination MAC address
    memcpy(frame.dst_addr, to, MAC_BYTES);

    // fill source MAC address
    memcpy(frame.src_addr, localMac, MAC_BYTES);

    // fill type
    frame.type = htons(type);

    // truncate if data is to long
    int data_size = strlen(data);
    if (data_size > MAX_ETHERNET_DATA_SIZE) {
        data_size = MAX_ETHERNET_DATA_SIZE;
    }

    // fill data
    memcpy(frame.data, data, data_size);

    int frame_size = ETHERNET_HEADER_SIZE + data_size;

    ret = sendto(s, &frame, frame_size, 0, NULL, 0);
    if (-1 == ret) {
        perror("send fail\n");
        return -1;
    }
    
    return 0;
}

int main(int argc, char *argv[]) 
{
    int sfd = -1;
    unsigned char to[6];
    char* dstMac = NULL;
    //short type;

    sfd = createEtherSocket(argv[1]);
    if (sfd < 0) {
        perror("Fail to create Ether Socket \n");
        exit(-1);
    }

    dstMac = getMacFromArp(argv[2]);
    if (NULL == dstMac) {
        fprintf(stderr, "Fail to get dest MAC address from ip %s \n", argv[2]);
        goto quit;
    }
    
    int ret = macAton(dstMac, to);
    if (0 != ret) {
        perror("Fail to mac aton \n");
        free(dstMac);
        goto quit;
    }

    free(dstMac);

    // send data
    ret = sendEtherData(sfd, to, TYPE, argv[3]);
    if (-1 == ret) {
        perror("Fail to send ethernet frame \n");
        goto quit;
    }
    
quit:
    close(sfd);

    return 0;
}
