#include "my_getopt.h"

void usage(char* exe)
{
	fprintf(stderr, "Invalid parameters! \n");
	fprintf(stderr, "Please input the corrects parameters! The correct samples are on below: \n");
	fprintf(stderr, "    %s -m RootCA \n", exe);
	fprintf(stderr, "    %s -m Device -l DICE \n", exe);
	fprintf(stderr, "    %s -m Device -l layer0 -s self_sign \n", exe);
	fprintf(stderr, "    %s -m Device -l layer0 -s root_sign \n", exe);
	fprintf(stderr, "    %s -m Device -l sublayers \n", exe);
}

int main(int argc, char** argv)
{
	char mode[10] = { 0 };
	char layer[10] = { 0 };
	char sign[10] = { 0 };
  
  while ((opt = getopt(argc, argv, "m:s:l:")) != -1) {
		switch (opt) {
		case 'm':
			strncpy(mode, optarg, sizeof(mode) -1);
			break;
		case 's':
			strncpy(sign, optarg, sizeof(sign) - 1);
			break;
		case 'l':
			strncpy(layer, optarg, sizeof(layer) - 1);
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
	}
  
  printf("mode: %s \n". mode);
  printf("layer: %s \n". mode);
  printf("sign: %s \n". mode);
}
