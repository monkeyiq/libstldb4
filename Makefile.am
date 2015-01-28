AUTOMAKE_OPTIONS = dist-bzip2 subdir-objects dejagnu

NULL=

SUBDIRS = db-4.6.21 STLdb4 testsuite 

EXTRA_DIST = 		\
stldb4.spec.in 		\
stldb4.spec             \
stldb4.pc.in            \
stldb4.pc               \
db-4.6.21

bin_SCRIPTS = stldb4-config

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = stldb4.pc

RUNTESTDEFAULTFLAGS = \
	--tool stldb4 EXBASE=`pwd`/ SDATA=`pwd`/testsuite/sampledata --srcdir $$srcdir/testsuite

dist-hook:
	touch $(distdir)/db-4.6.21/env/tags
