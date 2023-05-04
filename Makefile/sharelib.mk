CC ?= gcc
TEST ?= n
LIB := libmmio.so
EXE := mmio

ifeq ($(TEST), y)
all: $(EXE)
else
all: $(LIB)
endif

$(EXE):
        $(CC) mmio.c -DTEST -o mmio

$(LIB):
        $(CC) -fpic -shared mmio.c -o libmmio.so

clean:
        rm -f src/*.o $(LIB) $(EXE)
