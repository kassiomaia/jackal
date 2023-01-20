SUBDIRS=libjackal

BUILT_SOURCES=jackal_lexer.c jackal_parser.c ./include/jackal_parser.h
bin_PROGRAMS=jackal
jackal_SOURCES=jackal.c jackal_lexer.l jackal_parser.y
jackal_CFLAGS=-I$(top_srcdir)/include -L$(top_builddir)/libjackal
jackal_LDADD=-ljackal -lm
AM_YFLAGS=-d
