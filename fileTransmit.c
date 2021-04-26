#include "ethernet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


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
        //free(dstMac);
        goto quit;
    }

    //free(dstMac);

    // send data
    ret = sendEtherData(sfd, to, from, TYPE, argv[3]);
    if (-1 == ret) {
        perror("Fail to send ethernet frame: ");
        goto quit;
    }
    
quit:
    close(sfd);

    return 0;
}
