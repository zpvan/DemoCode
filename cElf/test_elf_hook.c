
#include <stdio.h>
#include <dlfcn.h>

#include "elf_hook.h"

#define LIBTEST1_PATH "libtest1.so"  //position dependent code (for 32 bit only)
#define LIBTEST2_PATH "libtest2.so"  //position independent code

int hooked_puts(char const *s)
{
    puts(s);  //calls the original puts() from libc.so because our main
    // executable module called "test" is intact by hook

    puts("is HOOKED!");
}

int main() {

    void *handle1 = dlopen(LIBTEST1_PATH, RTLD_LAZY);
    void *handle2 = dlopen(LIBTEST2_PATH, RTLD_LAZY);
    void *base1 = NULL, *base2 = NULL;
    void *original1, *original2;

    if (NULL == handle1 || NULL == handle2) {
        fprintf(stderr, "Failed to open \"%s\" or \"%s\" !\n", LIBTEST1_PATH, LIBTEST2_PATH);
        return 0;
    }

    libtest1();  // call puts() from libc.so twice
    libtest2();  // call puts() from libc.so twice
    puts("-----------------------------------------------------------");

    original1 = elf_hook(LIBTEST1_PATH, base1, "puts", hooked_puts);
    original2 = elf_hook(LIBTEST2_PATH, base2, "puts", hooked_puts);
    if (NULL == original1 || NULL == original2) {
        fprintf(stderr, "Redirection failed!\n");
    }

    libtest1();  // call hooked_puts() from libc.so twice
    libtest2();  // call hooked_puts() from libc.so twice
    puts("-----------------------------------------------------------");

    original1 = elf_hook(LIBTEST1_PATH, (const void *)LIBRARY_ADDRESS_BY_HANDLE(handle1), "puts", original1);
    original2 = elf_hook(LIBTEST2_PATH, (const void *)LIBRARY_ADDRESS_BY_HANDLE(handle2), "puts", original2);
    if (NULL == original1 || NULL == original2) {
        fprintf(stderr, "original Redirection failed!\n");
    }

    libtest1();  // call puts() from libc.so twice
    libtest2();  // call puts() from libc.so twice
    puts("-----------------------------------------------------------");

    dlclose(handle1);
    dlclose(handle2);

    return 0;
}