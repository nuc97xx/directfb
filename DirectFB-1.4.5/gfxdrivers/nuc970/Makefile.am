## Makefile.am for DirectFB/src/core/gfxcards/nuc970

INCLUDES = \
	-I$(top_builddir)/include	\
	-I$(top_srcdir)/include	\
	-I$(top_builddir)/lib	\
	-I$(top_srcdir)/lib	\
	-I$(top_srcdir)/src	\
	-I$(top_srcdir)/systems

nuc970_LTLIBRARIES = libdirectfb_nuc970.la

if BUILD_STATIC
nuc970_DATA = $(nuc970_LTLIBRARIES:.la=.o)
endif

nuc970dir = $(MODULEDIR)/gfxdrivers

libdirectfb_nuc970_la_SOURCES =	\
	nuc970_2d.c		\
	nuc970_2d.h		\
	nuc970_gfxdriver.c	\
	nuc970_gfxdriver.h

libdirectfb_nuc970_la_LDFLAGS = \
	-module			\
	-avoid-version		\

libdirectfb_nuc970_la_LIBADD = \
	$(top_builddir)/lib/direct/libdirect.la \
	$(top_builddir)/src/libdirectfb.la


include $(top_srcdir)/rules/libobject.make
	
