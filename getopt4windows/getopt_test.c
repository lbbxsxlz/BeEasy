#include "getopt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage(char* exe)
{
	fprintf(stderr, "Invalid parameters! \n");
	fprintf(stderr, "Please input the corrects parameters! The correct samples are on below: \n");
	fprintf(stderr, "    %s -m Server \n", exe);
	fprintf(stderr, "    %s -m Client -a DICE \n", exe);
	fprintf(stderr, "    %s -m Client -a 5 -b 7 \n", exe);
	fprintf(stderr, "    %s -m Client -a 8 -b 9 \n", exe);
	fprintf(stderr, "    %s -m Client -a 3 \n", exe);
}

int main(int argc, char** argv)
{
	int opt;
	int mode = 0;
	int a = 0, b = 0;
  
    while ((opt = getopt(argc, argv, "a:b:m")) != -1) {
		switch (opt) {
		case 'm':
			mode = 1;
			break;
		case 'a':
			a = atoi(optarg);
			break;
		case 'b':
			b = atoi(optarg);
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
	}

	if (mode != 1) {
		if (a == 0 || b == 0) {
			usage(argv[0]);
			exit(-1);
		}
	}

	return 0;
}
