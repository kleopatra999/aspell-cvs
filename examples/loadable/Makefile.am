export SED=@SED@
filterdir = ${pkglibdir}

INCLUDES = -I${top_srcdir}/interfaces/cc/ -I${top_srcdir}/common
FILTERVERSION = "0:1:0"
CXXFLAGS= @CXXFLAGS@ -DVERSION="\"${FILTERVERSION}\""
SUFFIXES = .opt

#filter_PROGRAMS = libcontext-filter.la
filter_LTLIBRARIES = libcontext-filter.la
filter_SCRIPTS=make_opt.awk
EXTRA_DIST = example_option_filter.opt make_opt.awk

libcontext_filter_la_SOURCES =	ccpp-context.h ccpp-context.cpp\
				loadable-filter-API.hpp
libcontext_filter_la_LIBADD = ${top_builddir}/lib/libaspell.la


optdir=${pkgdatadir}

context-filter.opt : ccpp-context.hpp ccpp-context.cpp
	$(AWK) -f $(srcdir)/make_opt.awk $^

opt_DATA=	context-filter.opt


