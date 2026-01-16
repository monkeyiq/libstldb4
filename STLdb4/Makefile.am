## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

DB4_PATH = $(top_srcdir)/db-4.6.21/dist

CFLAGS_EXTRA  = -I$(includedir) -I$(top_srcdir) -I/usr/local/include
LIBS_ALL      = $(LIBS_EXTRA)
CFLAGS_ALL    = $(CFLAGS_EXTRA)

INCLUDES	     = \
	-I$(DB4_PATH)                                   \
	-I. 						\
	-I$(top_srcdir)/STLdb4  			\
	-I$(srcdir) -I$(top_srcdir) 			\
	-I$(top_builddir)				\
        -I$(includedir)                                 \
	-I/usr/local/include                            \
       -DLIBDIR=\"$(libdir)\"   \
       -DBINDIR=\"$(bindir)\"   \
       -DDATADIR=\"$(datadir)\" \
	@LOKI_CFLAGS@           \
	@FSTREAM_CFLAGS@        \
	@CFLAGS@ 

EXTRA_DIST = 


lib_LTLIBRARIES        = libstldb4.la
libstldb4_includedir=$(includedir)/STLdb4
libstldb4_include_HEADERS        = \
	stldb4.hh HiddenSymbolSupport.hh stldbraw.h

libstldb4_la_SOURCES      =             \
	stldb4.cpp                      


libstldb4_la_LIBADD       = $(LIBS_ALL)

libstldb4_la_LDFLAGS      = @VERSION_INFO@ @LOKI_LIBS@ @FSTREAM_LIBS@ \
	\$(top_builddir)/db-4.6.21/build_unix/.libs/libdb-4.6.a
