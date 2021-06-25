SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, obj/%.o, $(SRC))
CC := gcc

TARGET := main

all : $(TARGET)

main: main.o c8583.o
	#$(CC) -o $@ $^ -g -fsanitize=address
	$(CC) -o $@ $^

$(OBJ):obj/%.o:src/%.c
	$(CC) -c -o $@ $< -I./include

clean:
	-rm $(OBJ) $(TARGET)

.PHONY: all clean  

vpath %.c  src
vpath %.o  obj
vpath %.h  include
