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

#define SUCCESS 0xE0
#define FAIL    0xEE

#define FILETRANSMIT 0x1024

typedef struct fileAttr {
	int status;
	uint64_t fileSize;
	char dstpath[MAXLEN];
	char dstfileName[MAXLEN];
}fileAttr_t;

typedef struct fileData {
	int status;
	unsigned int count;
	unsigned int len;
	unsigned int crc;
	unsigned char context[1484];
}fileData_t;


char filename[MAXLEN] = {0};
char filepath[MAXLEN] = {0};
uint64_t fileSize = 0;
FILE* fp = NULL;

static void usage(char *path)
{
    fprintf(stderr, "%s -c -i <iface> -t <ip> -f <filename> [-p <path>] \n", basename(path));
    fprintf(stderr, "%s -s -i <iface> \n", basename(path));
}

static unsigned int crcTable[256];
static void initCrcTable(void)  
{  
    unsigned int c;  
    unsigned int i, j;  
      
    for (i = 0; i < 256; i++) {  
        c = (unsigned int)i;  
        for (j = 0; j < 8; j++) {  
            if (c & 1)  
                c = 0xedb88320L ^ (c >> 1);  
            else  
                c = c >> 1;  
        }  
        crcTable[i] = c;  
    }  
}  
  
static unsigned int crc32(unsigned char *buffer, unsigned int size)  
{  
    unsigned int crc = 0xFFFFFFFF;
	unsigned int i;  
    for (i = 0; i < size; i++) {  
        crc = crcTable[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);  
    }  
    return crc;  
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
uint64_t htonll(uint64_t val)
{
	if (1 == htonl(1)) {
		return val;
	}

	return (((uint64_t)htonl(val)) << 32) + htonl(val>>32);
}

uint64_t ntohll(uint64_t val)
{
	if (1 == htonl(1)) {
		return val;
	}

	return (((uint64_t)ntohl(val)) << 32) + ntohl(val>>32);
}

int recvEtherFrame(int fd, ethernetFrame_t* frame)
{
	int ret;
	short type;
    
	ret = recvfrom(fd, frame, sizeof(ethernetFrame_t), 0, NULL, NULL);
	if (ret < 0) {
		perror("Fail to recv ether data");
		return -1;
	}

	type = ntohs(frame->type);
	if (type != FILETRANSMIT) {
		perror("Recv invalid date \n");
		return -1;
	}

	return 0;
}


int recvAck(int fd)
{
	int ret;
    ethernetFrame_t frame;
    int status;
    
	ret = recvEtherFrame(fd, &frame);
	if (ret < 0) {
		perror("recvEtherFrame fail \n");
		return -1;
	}
	
	memcpy(&status, frame.data, sizeof(status));
	status = ntohl(status);
	printf("status: 0x%x \n", status);

	return status;
}

int sendReq(int fd, unsigned char* to, unsigned char* from)
{
	fileAttr_t fattr;
    
    fattr.status = htonl(START);
	fattr.fileSize = htonll(fileSize);
    strncpy(fattr.dstpath, filepath, MAXLEN -1);
    strncpy(fattr.dstfileName, basename(filename), MAXLEN -1);

    return sendEtherData(fd, to, from, FILETRANSMIT, (char*)&fattr, sizeof(fattr));
}

int sendFileData(int fd, unsigned char* to, unsigned char* from)
{
	unsigned char buf[1484];
	fileData_t fData;
	int readByte = -1;
	unsigned int crc = 0;
	unsigned count = 0;
	int ret = -1;

	initCrcTable();

	while (!feof(fp)) {
		memset(&fData, 0, sizeof(fData));
		readByte = fread(buf, 1, sizeof(buf) -1, fp);
		crc = crc32(buf, readByte);

		printf("readByte = %d, crc = 0x%x \n", readByte, crc);
		
		fData.status = htonl(ING);
		fData.count = htonl(count);
		fData.len = htonl(readByte);
		fData.crc = htonl(crc);
		memcpy(fData.context, buf, readByte);
		
		ret = sendEtherData(fd, to, from, FILETRANSMIT, (char*)&fData, sizeof(fData));
		if (ret < 0) {
			fprintf(stderr, "%s sendEther fail \n", __func__);
			return -1;
		}

		ret = recvAck(fd);
		if (ret != SUCCESS) {
			fprintf(stderr,"%s recvAck fail \n", __func__);
			return -1;
		}

		count++;
	}

	return 0;
}

int fileSend(int fd, const char* iface, const char* dstIp)
{
	unsigned char from[6];
    unsigned char to[6];
    char dstMac[18] = {0};
    int ret = -1;
    int finish;

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
	
	fp = fopen(filename, "r");
	if (NULL == fp) {
		fprintf(stderr, "Fail to open file %s \n", filename);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	printf("fileSzie= %llu \n", fileSize);
	rewind(fp);

	ret = sendReq(fd, to, from);
	if (ret < 0) {
		perror("Fail to send req \n");
		goto quit;
    }

	ret = recvAck(fd);
    if (ret != SUCCESS) {
		perror("recvAck fail \n");
		goto quit;
    }

    ret = sendFileData(fd, to, from);
    if (ret < 0) {
		perror("sendFileData fail \n");
		goto quit;
    }

	finish = htonl(END);
	ret = sendEtherData(fd, to, from, FILETRANSMIT, (char*)&finish, sizeof(finish));
	if (ret < 0) {
		perror("sendEtherData finish fail \n");
		goto quit;
    }

    ret = recvAck(fd);
    if (ret != SUCCESS) {
		perror("recvAck fail \n");
		goto quit;
    }
   
quit:	
    fclose(fp);
	return ret;
}

int sendAck(int fd, ethernetFrame_t* frame, int stat)
{
	return sendEtherData(fd, frame->src_addr, frame->dst_addr, FILETRANSMIT, (char*)&stat, sizeof(stat));
}

int createFile(ethernetFrame_t* frame)
{
	char fName[MAXLEN] = {0};

	fileAttr_t fAttr;
	memcpy(&fAttr, frame->data, sizeof(fAttr));
	fileSize = ntohll(fAttr.fileSize);
	
	strncpy(filepath, fAttr.dstpath, MAXLEN - 1);
	strncpy(fName, fAttr.dstfileName, MAXLEN - 1);
	snprintf(filename, MAXLEN - 1, "%s%s", filepath, fName);

	printf("file: %s, fileSize = %llu \n", filename, fileSize);
	fp = fopen(filename, "a+");
	if (NULL == fp) {
		fprintf(stderr, "create file(%s) fail! \n", filename);
		return -1;
	}
#if 0
	/* create a empty file */
	if (fseek(fp, fileSize -1, SEEK_SET) < 0) {
		fclose(fp);
		return -1;
	}

	fputc('e', fp);
	fclose(fp);
#endif
	return 0;
}

int recvFileData(ethernetFrame_t *frame)
{
	fileData_t fData;
	unsigned char buf[1484] = {0};
	unsigned int count;
	unsigned len = 0;
	unsigned int crc = 0xFFFFFFFF;
	unsigned int crcCalc = 0xFFFFFFFF;
	int writeByte;

	memset(&fData, 0, sizeof(fData));
	memcpy(&fData, frame->data, sizeof(fData));
	count = ntohl(fData.count);
	len = ntohl(fData.len);
	crc = ntohl(fData.crc);

	memcpy(buf, fData.context, len);
	crcCalc = crc32(buf, len);

	printf("file size = %d, count = %d, crc = 0x%x, calcCrc = 0x%x \n", len, count, crc, crcCalc);
	
	if (crcCalc != crc) {
		perror("file data crc check fail \n");
		return -1;
	}

	writeByte = fwrite(buf, 1, len, fp);
	if (writeByte != len) {
		perror("fwrite fail \n");
	}

	return 0;
}

int fileRecv(int fd)
{
    int ret = -1;
	ethernetFrame_t frame;
    int status;
    unsigned int flag = 1;

    initCrcTable();

    while (flag) {
		ret = recvEtherFrame(fd, &frame);
		if (ret < 0) {
			perror("recvEtherFrame fail \n");
			return -1;
		}
		
		memcpy(&status, frame.data, sizeof(status));
		status = ntohl(status);
		printf("status: 0x%x \n", status);

		switch (status) {
			case START:
				ret = createFile(&frame);
				break;
			case ING:
				ret = recvFileData(&frame);
				break;

			case END:
				ret = fflush(fp);
				flag = 0;
				break;
				
			default:
				perror("invalid status \n");
				break;
        }
        if (ret < 0) {
			sendAck(fd, &frame, htonl(FAIL));
			goto quit;
		} else {
			sendAck(fd, &frame, htonl(SUCCESS));
		}
        usleep(1000);
	}
quit:
	if (fp)
    	fclose(fp);
	
	return 0;
}

int main(int argc, char ** argv)
{
    int opt;
    int isclent = -1;
    char iface[10] = {0};
    char ip[16] = {0};
    char buf[PATH_MAX] = {0};
    int sfd = -1;
    int ret = 0;
    char * str = NULL;

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
                str = realpath(optarg, buf);
	            if (!str) {
			       perror("realpath fail \n");
			       usage(argv[0]);
			       exit(-1);
		        }
		  		strncpy(filename, str, sizeof(filename)-1);
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
		    strncpy(filepath, filename, sizeof(filepath) - 1);
		    printf("src filename: %s; dst path: %s.\n", filename, dirname(filepath));
        }
    }

    //printf("htonll: 0x%lx, change: 0x%lx, change again \n", 0x00123456789A, htonll(0x00123456789A), ntohll(htonll(0x00123456789A)));

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
