# 编译动态库
gcc -g -m32 -masm=intel -shared -fPIC symbol.c -o libsymbol.so

# 分别编译两个版本main, 位置相关的main和位置无关的main_pi

# gcc默认-pie
# 位置相关
gcc -g -m32 -masm=intel -L. -lsymbol -no-pie -fno-pic main.c libsymbol.so -o main.out
# 位置无关
gcc -g -m32 -masm=intel -L. -lsymbol main.c libsymbol.so -o main_pi.out

# gcc默认-no-pie
# 位置相关
gcc -g -m32 -masm=intel -L. -lsymbol main.c libsymbol.so -o main.out
# 位置无关
gcc -g -m32 -masm=intel -L. -lsymbol -pie -fpic main.c libsymbol.so -o main_pi.out