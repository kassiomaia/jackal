SRCS=main.c
OBJS=$(SRCS:.c=.o)
CFLAGS=-g -I../include -L../lib -DVERBOSE -DENABLE_COLOR -Wl,-rpath=../lib 
TARGET=test

all: build_lib $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -ljackal
	./test

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

build_lib:
	make FLAGS="-DVERBOSE -DENABLE_COLOR" -C ../lib

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
