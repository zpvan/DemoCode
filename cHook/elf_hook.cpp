
#include <stdio.h>
#include <string.h>

typedef int (*fn_strlen)(const char *);

fn_strlen g_strlen = (fn_strlen)strlen;

int main(const int argc, const char *argv[]) {
    const char *elf_hook = "elf hook!";
    int a = g_strlen(elf_hook);
    int b = strlen(elf_hook);
    printf("a_len: %d, b_len: %d\n", a, b);
    return 0;
}