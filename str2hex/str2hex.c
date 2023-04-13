#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

/* ch change to hex, e.p. 1 -> 0x1, a to 0xa */
uint8_t get_base_value(char c)
{
	uint8_t value;
	if((c >= '0') && (c <= '9'))
		value = 48;
	else if ((c >= 'a') && (c <='f'))
		value = 87;
	else if ((c >= 'A') && (c <='F'))
		value = 55;
	else {
		printf("invalid data %c",c);
		return 255;
	}
	return value;
}
/* change function, two characters change to one hex byte */
void str_to_hex(char *str, size_t len, uint8_t *data)
{
	uint8_t sum = 0;
	uint8_t high = 0;
	uint8_t low = 0;
	uint8_t value = 0;
	size_t i, j = 0;

	for(i = 0; i < len; i++)
	{
		value = get_base_value(str[i]);
		high = (((str[i] - value) & 0xF) << 4);

		value = get_base_value(str[i + 1]);
		low = (str[i + 1] - value) & 0xF;

		sum = high | low;

		j = i / 2;
		data[j] = sum;
		i += 1;
	}
}

int create_binary_file(const char *src, const char *dst)
{
	FILE *fr = NULL;
	FILE *fw = NULL;
	long file_size;
	char *strbuf = NULL;
	uint8_t *binbuf = NULL;
	size_t str_len;
	size_t bin_len;
	size_t read_bytes;
	size_t write_bytes;
	int ret = 0;
	
	fr = fopen(src, "r");
	if (NULL == fr) {
		printf("Fail to open file %s, error: %s!\n", src, strerror(errno));
		return -1;
	}
	
	fw = fopen(dst, "wb");
	if (NULL == fw) {
		printf("Failed to open file %s, error: %s!\n", src, strerror(errno));
		ret = -1;
		goto quit;
	}
	
	(void)fseek(fr, 0, SEEK_END);
	file_size = ftell(fr);
	rewind(fr);
	
	str_len = file_size;
	bin_len = file_size / 2 + file_size % 2;
	
	strbuf = malloc(str_len + 1);
	if (NULL == strbuf) {
		printf("Failed to alloc memory, error: %s!\n", strerror(errno));
		ret = -1;
		goto quit;
	}
	memset(strbuf, 0, str_len + 1);
	
	binbuf = malloc(bin_len);
	if (NULL == binbuf) {
		printf("Failed to alloc memory, error: %s!\n", strerror(errno));
		ret = -1;
		goto quit;
	}
	memset(binbuf, 0, bin_len);
	
	read_bytes = fread(strbuf, 1, str_len, fr);
	if (read_bytes != str_len) {
		printf("Failed to read file %s, error: %s!\n", src, strerror(errno));
		ret = -1;
		goto quit;
	}
	
	str_to_hex(strbuf, str_len, binbuf);
	
	write_bytes = fwrite(binbuf, 1, bin_len, fw);
	if (write_bytes != bin_len) {
		printf("Failed to write file %s, error: %s!\n", dst, strerror(errno));
		ret = -1;
		goto quit;
	}
	
quit:
	if (fr)
		fclose(fr);
	if (fw)
		fclose(fw);
	if (strbuf)
		free(strbuf);
	if (binbuf)
		free(binbuf);
	
	return ret;
}

int main(int argc, char *argv[])
{
	char *s = "1b5050508af890ef50";
	uint8_t buf[10] = {0};
	size_t len = 0;
	printf("%s\n", s);
	str_to_hex(s, len, buf);
	for (int n = 0; n < len / 2; n++)
	{
		printf("0x%2X ", buf[n]);
	}
	printf("\n");
	
	if (argc < 3) {
		printf("invalid arguments!\n");
		printf("%s $string_file $binary_file.\n", argv[0]);
		return -1;
	}
	
	return create_binary_file(argv[1], argv[2]);
}
