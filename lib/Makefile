# wildcard of c files
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
CFLAGS=-I../include -L../lib -g
SHARED_LIB=libjackal.so
STATIC_LIB=libjackal.a

DEBUG=
export DEBUG

all: clean static shared

static: $(OBJS)
	ar rsv $(STATIC_LIB) $(OBJS)

shared: $(OBJS)
	$(CC) --shared -o $(SHARED_LIB) $(OBJS) $(DEBUG)

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@ $(DEBUG)

clean:
	rm -f $(OBJS) $(SHARED_LIB) $(STATIC_LIB)

.PHONY: all clean
