#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <getopt.h>

static void usage(char *path)
{
	fprintf(stderr, "\nInvalid parameters! \n");
	fprintf(stderr, "Please input the correct parmeters! The correct samples are on below: \n\n");
	fprintf(stderr, "%s -r -a $address \n", basename(path));
	fprintf(stderr, "%s -s -a $address -l $length \n", basename(path));
	fprintf(stderr, "%s -w -a $address -v $value \n", basename(path));
}

/* base16 to base10 */
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
	char mode[10] = {0};
	unsigned length;
	unsigned addr;
	unsigned value;

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
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	
	
	return 0;
}

