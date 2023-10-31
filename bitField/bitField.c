#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

typedef struct alpha {
        uint64_t a:14;
        uint64_t :2;
        uint64_t b:14;
        uint64_t :2;
        uint64_t c:4;
        uint64_t d:4;
        uint64_t :4;
        uint64_t e:17;
        uint64_t f:1;
        uint64_t g:2;
} alpha_t;


int main(int argc, char *argv[])
{
        alpha_t _abcd;
        uint64_t tmp = 0;

        memset(&_abcd, 0, sizeof(_abcd));
        printf("the size of alpha_t = %ld \n", sizeof(alpha_t));

        _abcd.f = 1;
        _abcd.g = 2;
        _abcd.a = 0x1234;

        memcpy(&tmp, &_abcd, sizeof(_abcd));

        printf("tmp = 0x%lx \n", tmp);
        return 0;
}
