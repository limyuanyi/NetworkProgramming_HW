CC = gcc
#除錯功能
CFLAGS = -Wno-implicit-function-declaration -g
# MySQL 编译和链接相关选项
MYSQL_LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lmysqlclient

SRCS = $(wildcard src/*.c)

OBJS = $(patsubst src/%.c, obj/%.o, ${SRCS})

# 指定clean , all 是 fake項目 
.PHONY: clean all

all: shell

shell: $(OBJS)
	$(CC) $^ $(MYSQL_LDFLAGS) -o ./bin/$@

obj/%.o: src/%.c 
	$(CC) ${CFLAGS} -c $^ -o $@

clean:
	@rm bin/* obj/*.o
