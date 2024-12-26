CC = gcc
#除錯功能
CFLAGS = -Wno-implicit-function-declaration -g

SRCS = $(wildcard src/*.c)

OBJS = $(patsubst src/%.c, obj/%.o, ${SRCS})

# 指定clean , all 是 fake項目 
.PHONY: clean all

all: shell

shell: $(OBJS)
	$(CC) $^ -o ./$@

obj/%.o: src/%.c 
	$(CC) ${CFLAGS} -c $^ -o $@

clean:
	@rm bin/* obj/*.o