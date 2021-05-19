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

#include "ethernet.h"

#if 0
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
#else 
int getMacFromArp(const char* req_ip, char* macStr)
{
    FILE *procFile;
    char ip[16];
    char mac[18];
 
    if (!(procFile = fopen("/proc/net/arp", "r"))) {
        return -1;
    }
 
    /* Skip first line */
    while (!feof(procFile) && fgetc(procFile) != '\n');
 
    while (!feof(procFile) && (fscanf(procFile, " %15[0-9.] %*s %*s %17[A-Fa-f0-9:] %*s %*s", ip, mac) == 2)) {
        if (strcmp(ip, req_ip) == 0) {
	        strcpy(macStr, mac);
	        break;
	    }
    }
 
    fclose(procFile);
    return 0;
}
#endif
 
int macAton(const char* a, unsigned char *n) 
{
    int matches = sscanf(a, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", n, n+1, n+2,
                         n+3, n+4, n+5);

    return (6 == matches ? 0 : -1);
}

int getIfindex(const char* iface, int* ifindex)
{
	int fd = -1;
	struct ifreq ifr;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		perror("Fail to create socket! \n");
		return -1;
	}

	strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name) - 1);
	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
		perror("Fail to get ifindex! \n");
		close(fd);
		return -1;
	}

	*ifindex = ifr.ifr_ifindex;

	close(fd);
	return 0;
}


int getLocalMacAddr(const char* iface, unsigned char* mac)
{
	int fd = -1;
	struct ifreq ifr;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		perror("Fail to create socket! \n");
		return -1;
	}
    
    strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name) - 1);

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) {
        perror("Fail to get mac address! \n");
		close(fd);
		return -1;
    }

    memcpy(mac, ifr.ifr_hwaddr.sa_data, MAC_BYTES);

    close(fd);
    return 0;
}

int createEtherSocket(const char* iface, short proto)
{
    int sfd = -1;
    int ret = -1;
    int if_index;

    sfd = socket(PF_PACKET, SOCK_RAW | SOCK_CLOEXEC, htons(proto));
    if (-1 == sfd) {
        return -1;
    }
    
    if (getIfindex(iface, &if_index) < 0) {
		return -1;
    }
   	
    struct sockaddr_ll sll;
    bzero(&sll, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_index;
    sll.sll_pkttype = PACKET_HOST;

    ret = bind(sfd, (struct sockaddr *)&sll, sizeof(sll));
    if (-1 == ret) {
        return -1;
    }

    return sfd;
}

int sendEtherData(int sfd, unsigned char* to, unsigned char* from, short type, const char* data, int data_size)
{
    int ret = -1;
    ethernetFrame_t frame;

    memcpy(frame.dst_addr, to, MAC_BYTES);
    memcpy(frame.src_addr, from, MAC_BYTES);
    frame.type = htons(type);

    memcpy(frame.data, data, data_size);

    if (data_size > MAX_ETHERNET_DATA_SIZE) {
        data_size = MAX_ETHERNET_DATA_SIZE;
    }

    int frame_size = ETHERNET_HEADER_SIZE + data_size;

    ret = sendto(sfd, &frame, frame_size, 0, NULL, 0);
    if (-1 == ret) {
        perror("send fail\n");
        return -1;
    }
    
    return 0;
}

