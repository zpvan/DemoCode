
default:
	make all

LIB_TEST1 = libtest1.so

$(LIB_TEST1):
	gcc -g3 -m32 -shared -o $(LIB_TEST1) libtest1.c

LIB_TEST2 = libtest2.so

$(LIB_TEST2):
	gcc -g3 -m32 -fPIC -shared -o $(LIB_TEST2) libtest2.c

BIN_TEST = test

$(BIN_TEST):
	gcc -g3 -m32 -L./ -o test test.c -ltest1 -ltest2 -ldl

all: $(LIB_TEST1) $(LIB_TEST2) $(BIN_TEST)