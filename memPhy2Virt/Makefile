#wildcard 扩展通配符
#patsubst 替换通配符
SRC_FILES := $(wildcard ./*.c)
OBJ_FILES := $(patsubst %.c,%.o,$(SRC_FILES))

INC := -I ./
BIN := memphy2virt
CC=$(HOST)-gcc

all: $(OBJ_FILES) $(BIN)

clean:
	rm -f $(OBJ_FILES) $(BIN)

#$@ 目标文件
#$< 第一个依赖文件
#$^ 所有的依赖文件	
%.o: %.c
	$(CC) $(INC) -c -o $@ $<
	
$(BIN): $(OBJ_FILES)
	$(CC) -o $@ $^
