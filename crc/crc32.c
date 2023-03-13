
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static uint32_t crc_table[256];

/* Initialize crc table */
static void init_crc_table(void)
{
        uint32_t c;
        uint32_t i, j;

        for (i = 0; i < 256; i++) {
                c = i;
                for (j = 0; j < 8; j++) {
                        if (c & 1) {
                                c = 0xedb88320L ^ (c >> 1);
                        } else {
                                c = c >> 1;
                        }
                }
                crc_table[i] = c;
        }
}

/*
 * Name:    CRC-32  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
 * Poly:    0x4C11DB7
 * Init:    0xFFFFFFF
 */
static uint32_t crc32_table(uint8_t *buf, uint16_t length)
{
        uint32_t crc = 0xffffffff;
        uint16_t i;

        for (i = 0; i < length; i++) {
                crc = crc_table[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
        }

        return ~crc;
}

/*
 * Name:    CRC-32  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
 * Poly:    0x4C11DB7
 * Init:    0xFFFFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFFFFF
 * Alias:   CRC_32/ADCCP
 * Use:  WinRAR,ect.
 */
uint32_t crc32(uint8_t *data, uint16_t length)
{
        uint8_t i;
        uint32_t crc = 0xffffffff;
        while(length--)
        {
                crc ^= *data;
                data++;
                for (i = 0; i < 8; ++i)
                {
                        if (crc & 1) {
                                /* 0xEDB88320= reverse 0x04C11DB7 */
                                crc = (crc >> 1) ^ 0xEDB88320;
                        } else {
                                crc = (crc >> 1);
                        }
                }
        }
        return ~crc;
}

/*
 * Name:    CRC-32/MPEG-2  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
 * Poly:    0x4C11DB7
 * Init:    0xFFFFFFF
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000000
 * Note:
*/
uint32_t crc32_mpeg_2(uint8_t *data, uint16_t length)
{
        uint8_t i;
        uint32_t crc = 0xffffffff;
        while(length--)
        {
                crc ^=(uint32_t)(*data)<<24;
                data++;
                for (i = 0; i < 8; ++i)
                {
                        if ( crc & 0x80000000 )
                                crc = (crc << 1) ^ 0x04C11DB7;
                        else
                                crc <<= 1;
                }
        }
        return crc;
}

int main(int argc, char *argv[])
{
        init_crc_table();
        printf("%x \n", crc32_table(argv[1], strlen(argv[1])));
        printf("%x \n", crc32(argv[1], strlen(argv[1])));

        printf("%x \n", crc32_mpeg_2(argv[1], strlen(argv[1])));
        return 0;
}
