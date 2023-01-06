INCLUDES=-Iinclude
LIBS=-Llib
DEBUG_FLAGS=-g
OPT_FLAGS=-O3
WARN_FLAGS=-Wall -Wextra -Werror
STD_FLAGS=-std=c99
PEDANTIC_FLAGS=-pedantic

FLAGS=
export FLAGS

CFLAGS=$(INCLUDES) $(LIBS) $(DEBUG_FLAGS) $(FLAGS)

TARGET=bin/jackal

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

all: clean parser lexer $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

parser: jackal_parser.y
	bison -d jackal_parser.y

lexer: jackal_lexer.l
	flex -o jackal_lexer.yy.c jackal_lexer.l

run: all
	$(TARGET) ./samples/main.jkl

gdb: all
	gdb --args $(TARGET) ./samples/main.jkl

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean run
