#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <getopt.h>
#include "memPhy2Virt.h"

static void usage(char *path)
{
	fprintf(stderr, "The samples are on below: \n\n");
	fprintf(stderr, "%s -r -a $address \n", basename(path));
	fprintf(stderr, "%s -s -a $address -l $length \n", basename(path));
	fprintf(stderr, "%s -w -a $address -v $value \n", basename(path));
}

unsigned long string2dec(void *nptr)
{
	char *p = (char *)nptr;

	if (NULL == p)
		return -1;

	if(strncmp("0x", p, 2)) {
		return strtoul(p, NULL, 0);
	}
	else {
		return strtoul(p, NULL, 16);
	}
	
    return 0;
}


int main(int argc, char* argv[])
{
	int opt;
	int ret;
	char mode[10] = {0};
	unsigned long length = 0;
	unsigned long addr = 0;
	unsigned long value = 0;
	
	unsigned int tmp_value = 0;
	unsigned int len = 0;

	while ((opt = getopt(argc, argv, "a:l:v:hrsw")) != -1) {
		switch (opt) {
		case 'h':
			usage(argv[0]);
			break;
		case 'a':
			addr = string2dec(optarg);
			break;
		case 'l':
			length = string2dec(optarg);
			break;
		case 'v':
			value = string2dec(optarg);
			break;
		case 'r':
			strncpy(mode, "read", sizeof(mode) -1);
		break;
		case 's':
			strncpy(mode, "show", sizeof(mode) -1);
			break;	
		case 'w':
			strncpy(mode, "write", sizeof(mode) -1);
			break;	
			
		default:
			fprintf(stderr, "\nInvalid parameters! \n");
			fprintf(stderr, "Please input the correct parmeters!"); 
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	
	if (strncmp(mode, "read", 4) == 0) {
		if (addr == 0) {
			fprintf(stderr, "\nInvalid parameters! \n");
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
		else {
			ret = readReg(addr, &tmp_value);
			if (ret < 0) {
				printf("read Reg[0x%lx] fail \n", addr);
				exit(EXIT_FAILURE);
			}
			value = tmp_value;
			printf("Reg[0x%lx] = 0x%lx \n", addr, value);
		}
	} else if (strncmp(mode, "write", 5) == 0) {
		if (addr == 0 || value == 0) {
			fprintf(stderr, "\nInvalid parameters! \n");
			usage(argv[0]);
			exit(EXIT_FAILURE);
		} else {
			tmp_value = value & 0xFFFFFFFF;
			ret = writeReg(addr, tmp_value);
			if (ret < 0) {
				printf("write Reg[0x%lx] fail \n", addr);
				exit(EXIT_FAILURE);
			}
		}	
	} else if (strncmp(mode, "show", 4) == 0) {
		if (addr == 0 || length == 0) {
			fprintf(stderr, "\nInvalid parameters! \n");
			usage(argv[0]);
			exit(EXIT_FAILURE);
		} else {
			len = length & 0xFFFFFFFF;
			ret = showRegValue(addr, len);
			if (ret < 0) {
				printf("show Reg[0x%lx] fail \n", addr);
				exit(EXIT_FAILURE);
			}
		}
	}
	else {
		fprintf(stderr, "\nInvalid parameters! \n");
		fprintf(stderr, "Please input the correct parmeters!"); 
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	
	return 0;
}
