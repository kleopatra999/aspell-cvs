#export SED=@SED@
export PERLPROG=@PERLPROG@
filterdir = ${pkglibdir}

INCLUDES = -I${top_srcdir}/interfaces/cc/ -I${top_srcdir}/common 
INCLUDES+= -I${top_srcdir}/interfaces/filter
FILTERVERSION = "0:1:0"
CXXFLAGS= @CXXFLAGS@ -DVERSION="\"${FILTERVERSION}\""
SUFFIXES = .opt

#filter_PROGRAMS = libcontext-filter.la
filter_LTLIBRARIES = libcontext-filter.la
EXTRA_DIST = example_option_filter.opt 

libcontext_filter_la_SOURCES =	ccpp-context.hpp ccpp-context.cpp
libcontext_filter_la_LIBADD = ${top_builddir}/lib/libaspell.la


optdir=${pkgdatadir}

context-filter.opt : ${libcontext_filter_la_SOURCES}
	${PERLPROG} ${top_srcdir}/scripts/mk-filter-opt.pl $^

opt_DATA= context-filter.opt
