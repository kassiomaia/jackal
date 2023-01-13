CFLAGS=-g -I../include -L../lib -DVERBOSE -DENABLE_COLOR -Wl,-rpath=../lib 
TARGET=../bin/jackaltest

compiler: build_lib
	$(CC) $(CFLAGS) -o $(TARGET) main.c compiler.c -ljackal -lm -lcheck
	./$(TARGET)

build_lib:
	make FLAGS="-DVERBOSE -DENABLE_COLOR" -C ../lib

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
