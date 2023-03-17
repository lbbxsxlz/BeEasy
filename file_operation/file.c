static int write_file(int rd, const char *file)
{
	char buf[1024];
	ssize_t ret;

	int fw = open(file, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fw < 0) {
		fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
		return -1;
	}

	while ((ret = read(rd, buf, 1024)) > 0) {
		(void)write(fw, buf, ret);
	}

	close(fw);
	return 0;
}

static int copy_file(const char *dst, const char *src)
{
	int ret = 0;
	FILE *fdst = NULL;
	FILE *fsrc = NULL;

	uint8_t tempbuf[4096] = {0};
	size_t readBytes = -1;
	size_t writeBytes = -1;

	fdst = fopen(dst, "wb");
	if (fdst == NULL) {
		fprintf(stderr, "fopen file %s fail %s!\n", dst, strerror(errno));
		return -1;
	}

	fsrc = fopen(src, "rb");
	if (fsrc == NULL) {
		fprintf(stderr, "fopen file %s fail %s!\n", src, strerror(errno));
		ret = -1;
		goto quit;
	}

	while (!feof(fsrc)) {
		readBytes = fread(tempbuf, 1, sizeof(tempbuf) - 1, fsrc);

		writeBytes = fwrite(tempbuf, 1, readBytes, fdst);
		if (writeBytes != readBytes) {
			fprintf(stderr, "Failed to write file %s!\n", strerror(errno));
			ret = -1;
			goto quit;
		}
	}

	fflush(fdst);

quit:
	if (fdst) {
		fclose(fdst);
	}

	if (fsrc) {
		fclose(fsrc);
	}

	return ret;
}

static void *mmap_file(const char *filename, size_t *size)
{
	int fd = -1;
	struct stat file_stat;
	void *pointer = NULL;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
		goto quit;
	}

	if (fstat(fd, &file_stat) < 0) {
		fprintf(stderr, "Failed to get file status: %s\n", strerror(errno));
		goto quit;
	}

	*size = file_stat.st_size;

	pointer = mmap((void *)0, *size, PROT_READ, MAP_SHARED, fd, 0);
	if (pointer == MAP_FAILED) {
		fprintf(stderr, "Failed to mmap file: %s\n", strerror(errno));
		pointer = NULL;
		goto quit;
	}

quit:
	if (fd > 0) {
		close(fd);
	}

	return pointer;
}

static int unmap_file(void *addr, size_t length)
{
	if (munmap(addr, length) < 0) {
		fprintf(stderr, "Failed to munmap file: %s\n", strerror(errno));
		return -1;
	}

	addr = NULL;
	return 0;
}