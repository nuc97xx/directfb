## Makefile.am for DirectFB/inputdrivers/wm97xx

INCLUDES = \
	-I$(top_srcdir)/include	\
	-I$(top_builddir)/lib	\
	-I$(top_srcdir)/lib	\
	-I$(top_srcdir)/src

input_LTLIBRARIES = libdirectfb_wm97xx_ts.la

if BUILD_STATIC
input_DATA = $(input_LTLIBRARIES:.la=.o)
endif

inputdir = $(MODULEDIR)/inputdrivers

libdirectfb_wm97xx_ts_la_SOURCES =	\
	wm97xx_ts.c

libdirectfb_wm97xx_ts_la_LDFLAGS = \
	-module			\
	-avoid-version		\
	$(DFB_LDFLAGS)

libdirectfb_wm97xx_ts_la_LIBADD = \
	$(top_builddir)/lib/direct/libdirect.la \
	$(top_builddir)/src/libdirectfb.la


include $(top_srcdir)/rules/libobject.make
	
