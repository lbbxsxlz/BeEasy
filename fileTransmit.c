#include "ethernet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#define MAXLEN 128

struct fileData {
	unsigned int count;
	unsigned crc;
	char context[];
};

char filename[MAXLEN] = {0};
char filepath[MAXLEN] = {0};

static void usage(char *path)
{
    fprintf(stderr, "%s -c -i <iface> -t <ip> -f <filename> -p <path> \n", basename(path));
    fprintf(stderr, "%s -s -i <iface> \n", basename(path));
}


#if 0
int main(int argc, char *argv[]) 
{
    int sfd;
    unsigned char from[6];
    unsigned char to[6];
    //char* dstMac = NULL;
    char dstMac[18] = {0};

    sfd = createEtherSocket(argv[1], TYPE);
    if (sfd < 0) {
        perror("Fail to create Ether Socket \n");
        exit(-1);
    }

    if (getLocalMacAddr(argv[1], from) < 0) {
		perror("Fail to get local mac address \n");
		goto quit;
    }
#if 0
    dstMac = getMacFromArp(argv[2]);
    if (NULL == dstMac) {
    	fprintf(stderr, "Fail to get dest MAC address from ip %s \n", argv[2]);
    	goto quit;
    }
#else
	if (getMacFromArp(argv[2], dstMac) < 0) {
		perror("Fail to get dest mac address \n");
		goto quit;
	}
#endif

    int ret = macAton(dstMac, to);
    if (0 != ret) {
        perror("Fail to mac aton \n");
        goto quit;
    }

    ret = sendEtherData(sfd, to, from, TYPE, argv[3]);
    if (-1 == ret) {
        perror("Fail to send ethernet frame: ");
        goto quit;
    }
    
quit:
    close(sfd);
}
#endif

int fileSend(const char* iface, const char* dst)
{

}

int fileRecv(const char* iface)
{
	
}

int main(int argc, char * argv[])
{
    int opt;
    unsigned int isclent = 0;
    char iface[10] = {0};
    char ip[16] = {0};
	
    while ((opt = getopt(argc, argv, "f:p:t:i:cs")) != -1) {
        switch (opt) {
           case 's':
               isclent = 0;
               break;
           case 'c':
               isclent = 1;
               break;
           case 'i':
               strncpy(iface, optarg, sizeof(iface) - 1);
               break;
           case 't':
               strncpy(ip, optarg, sizeof(ip) - 1);
               break;
           case 'f':
               strncpy(filename, optarg, sizeof(filename) - 1);
               break;
           case 'p':
               strncpy(filepath, optarg, sizeof(filepath) - 1);
               break;

           default:
               usage(argv[0]);
        }
    }

    if (isclent) {
		return fileSend(iface, ip);
    }
    else {
		return fileRecv(iface);
    }
    
    return 0;
}
