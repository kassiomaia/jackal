CFLAGS=-g -I../include -L../lib -DVERBOSE -DENABLE_COLOR -Wl,-rpath=../lib 
TARGET=../bin/jackaltest

compiler: build_lib
	@echo "Building jackal compiler tests..."
	$(CC) $(CFLAGS) -o $(TARGET) main.c compiler.c -ljackal -lm -lcheck
	@echo "Running jackal compiler tests..."
	@echo "=============================================================="
	./$(TARGET)
	@if [ $$? -ne 0 ]; then \
		echo "=============================================================="; \
		echo "Jackal compiler tests failed!"; \
		exit 1; \
	else \
		echo "=============================================================="; \
		echo "Jackal compiler tests passed!"; \
	fi
	@echo "=============================================================="
	

build_lib:
	make FLAGS="-DVERBOSE -DENABLE_COLOR" -C ../lib

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
