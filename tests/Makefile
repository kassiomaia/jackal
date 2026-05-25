# Compiler unit tests.
#
# Uses the "Check" framework when available (pkg-config); otherwise falls back
# to the dependency-free shim in no_check.h (-DJKL_NO_CHECK). Library sources
# are compiled directly so the tests do not depend on the autotools build.

CC      ?= cc
CFLAGS   = -g -I../include
LIBSRC   = $(wildcard ../libjackal/*.c)
TESTSRC  = main.c compiler.c
TARGET   = jackaltest

# Integration tests drive the real flex/bison front end (committed generated
# sources) and run under AddressSanitizer to catch leaks / invalid frees.
PARSERSRC = ../jackal_parser.c ../jackal_lexer.c
INTEGSRC  = precedence.c
INTEGTGT  = jackalinteg
ASANFLAGS = -fsanitize=address

HAVE_CHECK := $(shell pkg-config --exists check 2>/dev/null && echo yes)

ifeq ($(HAVE_CHECK),yes)
  TESTDEFS =
  TESTLIBS = -lcheck
else
  TESTDEFS = -DJKL_NO_CHECK
  TESTLIBS =
endif

compiler:
	@echo "Building jackal compiler tests (HAVE_CHECK=$(HAVE_CHECK))..."
	$(CC) $(CFLAGS) $(TESTDEFS) -o $(TARGET) $(TESTSRC) $(LIBSRC) -lm $(TESTLIBS)
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

precedence:
	@echo "Building jackal integration tests (ASan)..."
	$(CC) $(CFLAGS) $(ASANFLAGS) -I.. -o $(INTEGTGT) $(INTEGSRC) $(PARSERSRC) $(LIBSRC) -lm
	@echo "Running jackal integration tests..."
	@echo "=============================================================="
	./$(INTEGTGT)
	@if [ $$? -ne 0 ]; then \
		echo "=============================================================="; \
		echo "Jackal integration tests failed!"; \
		exit 1; \
	else \
		echo "=============================================================="; \
		echo "Jackal integration tests passed!"; \
	fi

clean:
	rm -f $(TARGET) $(INTEGTGT)

.PHONY: compiler precedence clean
