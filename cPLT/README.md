gcc -m32 -fPIC -c libdummy.c
ld -m elf_i386 -shared -soname libdummy.so.1 -o libdummy.so.1.0 -lc libdummy.o

ldconfig -v -n .

ln -sf libdummy.so.1 libdummy.so

gcc -m32 -o dummyelf.out dummyelf.c -L. -ldummy
