SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, obj/%.o, $(SRC))
CC := gcc

TARGET := main

all : obj $(TARGET)

main: main.o c8583.o
#	$(CC) -o $@ $^ -g -fsanitize=address
	$(CC) -o $@ $^

$(OBJ):obj/%.o:src/%.c
	$(CC) -c -o $@ $< -I./include

obj:
	@mkdir $@

clean:
	-rm $(OBJ) $(TARGET)
	@rmdir obj

.PHONY: all clean  

vpath %.c  src
vpath %.o  obj
vpath %.h  include
