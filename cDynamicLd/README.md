// Lib.so
gcc -m32 -fPIC -shared -o Lib.so Lib.c

// Program1.out
gcc -m32 -o Program1.out Program1.c ./Lib.so

// Program2.out
// Program1.out
gcc -m32 -o Program2.out Program2.c ./Lib.so