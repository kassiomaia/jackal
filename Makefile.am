SUBDIRS=libjackal

BUILT_SOURCES=jackal_lexer.c jackal_parser.c ./include/jackal_parser.h
bin_PROGRAMS=jackal
jackal_SOURCES=jackal.c jackal_lexer.l jackal_parser.y
jackal_CFLAGS=-I$(top_srcdir)/include -L$(top_builddir)/libjackal
jackal_LDADD=-ljackal -lm
AM_YFLAGS=-d

# The tests in tests/ are intentionally standalone (they compile libjackal/*.c
# directly and fall back to a Check-free shim). Drive them from `make check`
# without converting them to automake (tests/ stays out of SUBDIRS).
check-local:
	$(MAKE) -C $(top_srcdir)/tests compiler
	$(MAKE) -C $(top_srcdir)/tests precedence
