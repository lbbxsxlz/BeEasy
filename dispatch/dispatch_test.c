#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef int (*test_func_t) (int argc, char *argv[]);

typedef struct {
	char test_name[10];
	test_func_t func;
} test_dispatch_t;

static int example1(int argc, char *argv[]);
static int example2(int argc, char *argv[]);
static int example3(int argc, char *argv[]);
static int example4(int argc, char *argv[]);
static int example5(int argc, char *argv[]);
static int example6(int argc, char *argv[]);


test_dispatch_t test_dispatch[] = {
	{"test1", example1},
	{"test2", example2},
	{"test3", example3},
	{"test4", example4},
	{"test5", example5},
	{"test6", example6},
};

static int example1(int argc, char *argv[])
{
	printf("example1!\n");
	return 0;
}

static int example2(int argc, char *argv[])
{
	printf("example2!\n");
	return 0;
}

static int example3(int argc, char *argv[])
{
	printf("example3!\n");
	return 0;
}

static int example4(int argc, char *argv[])
{
	printf("example4!\n");
	return 0;
}

static int example5(int argc, char *argv[])
{
	printf("example5!\n");
	return 0;
}

static int example6(int argc, char *argv[])
{
	printf("example6!\n");
	return 0;
}

int main(int argc, char **argv)
{
	int i;
	int ret;
 
	if (argc < 2) {
		printf("invalid arguments!\n");
		return -1;
	}

	for (i = 0; i < sizeof(test_dispatch) / sizeof(test_dispatch[0]); i++) {
		if (strncmp(argv[1], test_dispatch[i].test_name, sizeof(test_dispatch[i].test_name) - 1) == 0) {
			ret = test_dispatch[i].func(argc - 1, argv + 1);
			break;
		}
	}

	if (i == sizeof(test_dispatch) / sizeof(test_dispatch[0])) {
		printf("invalid arguments!\n");
		return -1;
	}

	return 0;
}
