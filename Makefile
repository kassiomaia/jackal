TARGET=bin/jackal

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
DEBUG=-fsanitize=address -fsanitize=undefined -g -DVERBOSE -DENABLE_COLOR
CFLAGS=-Iinclude -Llib -lm -ljackal -Wl,-rpath=lib
GDB=gdb --args
ASTYLE=/home/kassio/projects/astyle/build/gcc/bin/astyle

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

run: CFLAGS += -DVERBOSE -DENABLE_COLOR -fsanitize=address -fsanitize=undefined -g
run: all
	$(TARGET) ./samples/main.jkl

test:
	$(MAKE) -C tests

format:
	$(ASTYLE) --style=linux --indent=spaces=2 --indent-switches --indent-preproc-define --indent-preproc-cond --indent-col1-comments --pad-oper --pad-header --unpad-paren --align-pointer=name --align-reference=name --add-brackets --convert-tabs --close-templates --max-code-length=80 --suffix=none --mode=c lib/*.c include/**/*.h

defs:
	gperf -t -L ANSI-C -C -E -k '1,3,$$' -H hash -N keyword_lookup -p -t -j1 -o -i 1 -g -G -s 2 ./defs/keywords.gperf > ./include/jackal/jackal_keywords.h

cppcheck:
	cppcheck -I include lib/*.c

.PHONY: all clean run gdb jackal run test format defs

