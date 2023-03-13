#include <stdio.h>
#include <stdlib.h>

int main()
{
        char *key[8];
        char *word[8];
        int i;
        int column = 32;

        for (i = 0; i < 8; i++) {
                key[i] = (char*)malloc(sizeof(char) * column);
        }

        printf("%d\n", sizeof(key));

        for (i = 0; i < 8; i++) {
                printf("%x \n", key[i]);

                free(key[i]);
        }

        word[0] = malloc(8 * sizeof(char) *column);

        printf("%d\n", sizeof(word));
        for (i = 1; i < 8; i++) {
                word[i] = word[i - 1] + column;
        }

        for (i = 0; i < 8; i++) {
                printf("%x \n", word[i]);
        }

        free(word[0]);
}
