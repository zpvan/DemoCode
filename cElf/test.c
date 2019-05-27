
#include <stdio.h>
#include <dlfcn.h>

#define LIBTEST1_PATH "libtest1.so"  // position dependent code (for 32 bit only)
#define LIBTEST2_PATH "libtest2.so"  // position independent code

void libtest1();
void libtest2();

int main() {

    void *handle1 = dlopen(LIBTEST1_PATH, RTLD_LAZY);
    void *handle2 = dlopen(LIBTEST2_PATH, RTLD_LAZY);

    if (NULL == handle1 || NULL == handle2) {
        fprintf(stderr, "Failed to open \"%s\" or \"%s\" !\n", LIBTEST1_PATH, LIBTEST2_PATH);
        return 0;
    }

    libtest1();  // call puts() from libc.so twice
    libtest2();  // call puts() from libc.so twice
    puts("-----------------------------------------------------------");

    dlclose(handle1);
    dlclose(handle2);

    return 0;
}