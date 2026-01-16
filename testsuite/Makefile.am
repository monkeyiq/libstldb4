NULL=
SUBDIRS = \
	config                      \
	stldb4.test                 \
	sampledata                  

DB4_PATH = $(top_srcdir)/db-4.1.25/dist
#INCLUDES = 
AM_CXXFLAGS =                                              \
	-I$(DB4_PATH)                                   \
	-I. 						\
	-I$(srcdir) -I$(top_srcdir) -I$(top_srcdir)/STLdb4 \
	-I$(top_builddir)				\
        -I$(includedir)                                 \
	@LOKI_CFLAGS@                                   \
	@FSTREAM_CFLAGS@                                \
	@CFLAGS@

noinst_PROGRAMS = \
	ut_setget ut_forwarditerate ut_find ut_overload ut_partial_lookup \
	ut_readstream ut_writestream ut_readwriteseek \
	ut_multidata ut_trans ut_trans_manydb ut_reverseiterate ut_partial_range_lookup \
	ut_recno ut_secondary ut_advancef ut_secondary_recno ut_secondary_manyruns

ut_setget_SOURCES = ut_setget.cpp
ut_setget_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_setget_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@
ut_setget_CFLAGS  = $(INCLUDES)

ut_forwarditerate_SOURCES = ut_forwarditerate.cpp
ut_forwarditerate_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_forwarditerate_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_find_SOURCES = ut_find.cpp
ut_find_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_find_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_overload_SOURCES = ut_overload.cpp
ut_overload_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_overload_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_partial_lookup_SOURCES = ut_partial_lookup.cpp
ut_partial_lookup_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_partial_lookup_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_readstream_SOURCES = ut_readstream.cpp
ut_readstream_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_readstream_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_writestream_SOURCES = ut_writestream.cpp
ut_writestream_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_writestream_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_readwriteseek_SOURCES = ut_readwriteseek.cpp
ut_readwriteseek_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_readwriteseek_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_multidata_SOURCES = ut_multidata.cpp
ut_multidata_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_multidata_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_trans_SOURCES = ut_trans.cpp
ut_trans_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_trans_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_trans_manydb_SOURCES = ut_trans_manydb.cpp
ut_trans_manydb_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_trans_manydb_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_reverseiterate_SOURCES = ut_reverseiterate.cpp
ut_reverseiterate_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_reverseiterate_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_partial_range_lookup_SOURCES = ut_partial_range_lookup.cpp
ut_partial_range_lookup_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_partial_range_lookup_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_recno_SOURCES = ut_recno.cpp
ut_recno_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_recno_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_secondary_SOURCES = ut_secondary.cpp
ut_secondary_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_secondary_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_secondary_recno_SOURCES = ut_secondary_recno.cpp
ut_secondary_recno_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_secondary_recno_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_advancef_SOURCES = ut_advancef.cpp
ut_advancef_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_advancef_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@

ut_secondary_manyruns_SOURCES = ut_secondary_manyruns.cpp
ut_secondary_manyruns_LDADD   = -lpopt   @LIBSTLDB4_LA@ 
ut_secondary_manyruns_LDFLAGS =   @LOKI_LIBS@ @FSTREAM_LIBS@


