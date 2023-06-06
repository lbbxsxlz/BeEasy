#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

size_t length;

int realloc_memory(size_t *size, void ***pointer)
{
	void *np;
	void *oldPointer;

	oldPointer = **pointer;

	np = realloc(oldPointer, (*size) * 2);
	if (NULL == np) {
		return -1;
	}
	*pointer = &np;
	printf("address after realloc: %p\n", **pointer);

	return 0;
}

int main(int argc, char *argv[])
{
	void **addr;
	void *p = NULL;
	int ret;

	if (argc < 2) {
		length = 128;
	} else {
		length = strtoul(argv[1], NULL, 10);
	}

	p = malloc(length);
	if (NULL == p) {
		perror("malloc fail!\n");
		return -1;
	}
	addr = &p;

	printf("address after malloc: %p\n", *addr);
	ret = realloc_memory(&length, &addr);
	if (ret != 0) {
		goto quit;
	}

quit:
	free(*addr);
	return ret;
}
