#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MB (1024*1024)

int main(int argc, char *argv[]) {
    char *p = NULL;
    int i = 0;
    while (1) {
        p = malloc(MB);
        memset(p, 0, MB);
        printf("%dM memory allocated!\n", ++i);
        sleep(1);
    }
    return 0;
}