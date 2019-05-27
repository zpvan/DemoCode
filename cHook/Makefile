
.PHONY: clean all

default:
	make all

LIB_TARGET = libcmath.so
$(LIB_TARGET):
	g++ Calculator.cpp -o $(LIB_TARGET) -fPIC -shared

OBJ_TARGET = hookMain
$(OBJ_TARGET):
	g++ hook_main.cpp -o $(OBJ_TARGET) -L./ -lcmath

OBJ_TARGET2 = elfHook
$(OBJ_TARGET2):
	g++ elf_hook.cpp -o $(OBJ_TARGET2)

all: $(LIB_TARGET) $(OBJ_TARGET) $(OBJ_TARGET2)