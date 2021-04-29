#include "ethernet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <arpa/inet.h>


#define MAXLEN 128
#define START 0x05
#define END  0x0A
#define ING  0x5A

#define FILETRANSMIT 0x0512

struct fileAttr {
	int status;
	char dstpath[MAXLEN];
	char dstfileName[MAXLEN];
};

struct fileData {
	int status;
	unsigned int count;
	unsigned int crc;
	char context[1488];
};


char filename[MAXLEN] = {0};
char filepath[MAXLEN] = {0};

static void usage(char *path)
{
    fprintf(stderr, "%s -c -i <iface> -t <ip> -f <filename> [-p <path>] \n", basename(path));
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

int sendReq(int fd, unsigned char* to, unsigned char* from)
{
	struct fileAttr fattr;
    
    fattr.status = htonl(START);
    strncpy(fattr.dstpath, filepath, MAXLEN -1);
    strncpy(fattr.dstfileName, basename(filename), MAXLEN -1);

    return sendEtherData(fd, to, from, FILETRANSMIT, (char*)&fattr, sizeof(fattr));
}

int fileSend(int fd, const char* iface, const char* dstIp)
{
	unsigned char from[6];
    unsigned char to[6];
    char dstMac[18] = {0};

	FILE* fp = NULL;
	long size;

	if (getLocalMacAddr(iface, from) < 0) {
		perror("Fail to get local mac address \n");
		return -1;
    }

    if (getMacFromArp(dstIp, dstMac) < 0) {
		perror("Fail to get dest mac address \n");
		return -1;
	}

    if (macAton(dstMac, to) != 0) {
        perror("Fail to mac aton \n");
		return -1;
    }

    if (sendReq(fd, to, from) < 0) {
		perror("Fail to send req \n");
		return -1;
    }
	
    return 0;
/*
	fp = fopen(filename, "r");
	if (NULL == fp) {
		fprintf(stderr, "Fail to open file %s \n", filename);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);
	
	return 0;
*/	
}

int fileRecv(int fd)
{
	return 0;
}

int main(int argc, char ** argv)
{
    int opt;
    int isclent = -1;
    char iface[10] = {0};
    char ip[16] = {0};
    int sfd = -1;
    int ret = 0;

    if (argc < 2) {
		usage(argv[0]);
		exit(-1);
    }
	
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
               realpath(optarg, filename);
               break;
           case 'p':
               strncpy(filepath, optarg, sizeof(filepath) - 1);
               break;

           default:
               usage(argv[0]);
               exit(-1);
        }
    }

    if (-1 == isclent || 0 == strlen(iface)) {
		usage(argv[0]);
		exit(-1);
    }

    if (isclent) {
		if (0 == strlen(filename) || 0 == strlen(ip)) {
			usage(argv[0]);
			exit(-1);
    	}

    	if (0 == strlen(filepath)) {
		    strncpy(filepath, dirname(filename), sizeof(filepath) - 1);
        }
    }

    sfd = createEtherSocket(iface, FILETRANSMIT);
    if (sfd < 0) {
        perror("Fail to create Ether Socket \n");
        exit(-1);
    }

    if (isclent) {
		ret = fileSend(sfd, iface, ip);
		if (ret < 0) {
			perror("fileSend fail! \n");
		}
    }
    else {
		ret = fileRecv(sfd);
		if (ret < 0) {
			perror("fileRecv fail! \n");
		}
    }

    close(sfd);
    return ret;
}
