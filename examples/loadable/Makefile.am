filterdir = "$(libdir)/aspell"

INCLUDES = -I${top_srcdir}/interfaces/cc/ -I${top_srcdir}/common
FILTERVERSION = "0:0:1"
CXXFLAGS= @CXXFLAGS@ -DVERSION="\"${FILTERVERSION}\""
SUFFIXES = .opt

filter_PROGRAMS = libcontext-filter.la
filter_SCRIPTS=make_opt.awk
EXTRA_DIST = example_option_filter.opt make_opt.awk

libcontext_filter_la_SOURCES =	ccpp-context.h ccpp-context.cpp\
				loadable-filter-API.hpp
libcontext_filter_la_LDFLAGS = -laspell


optdir=${pkgdatadir}

context-filter.opt : ccpp-context.hpp ccpp-context.cpp
	$(AWK) -f $(srcdir)/make_opt.awk $^

opt_DATA=	context-filter.opt


