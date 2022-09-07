#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <getopt.h>

static void usage(char *path)
{
        fprintf(stderr, "%s [ -l <len> ] [ -j <journal size> ] [ -b <block_size> ]\n", basename(path));
        fprintf(stderr, "    [ -g <blocks per group> ] [ -i <inodes> ] [ -I <inode size> ]\n");
        fprintf(stderr, "    [ -L <label> ] [ -f ] [ -a <android mountpoint> ]\n");
        fprintf(stderr, "    [ -z | -s ] [ -J ]\n");
        fprintf(stderr, "    <filename> [<directory>]\n");
}

/* base16 to base10 */
int string2dec(void *nptr)
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

	while ((opt = getopt(argc, argv, "h:m")) != -1) {
        switch (opt) {
        case 'h':
                usage(argv[0]);
                break;
        case 'm':
                break;
        default: 
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}

