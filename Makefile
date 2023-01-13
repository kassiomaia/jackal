TARGET=bin/jackal

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
DEBUG=-fsanitize=address -fsanitize=undefined -g -DVERBOSE -DENABLE_COLOR
CFLAGS=-Iinclude -Llib -lm -ljackal -Wl,-rpath=lib
GDB=gdb --args

all: jackal clean parser lexer $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

parser: jackal_parser.y
	bison -d jackal_parser.y

lexer: jackal_lexer.l
	flex -o jackal_lexer.yy.c jackal_lexer.l

clean:
	rm -f $(OBJS) $(TARGET)

gdb: all
	$(GDB) $(TARGET) ./samples/main.jkl

jackal:
	$(MAKE) -e DEBUG="-DVERBOSE -DENABLE_COLOR" -C lib

run: CFLAGS += -DVERBOSE -DENABLE_COLOR
run: all
	$(TARGET) ./samples/main.jkl

test:
	$(MAKE) -C tests

.PHONY: all clean run gdb jackal run

