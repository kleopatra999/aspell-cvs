export SED = @SED@
INCLUDES = -I${top_srcdir}/common -I${top_srcdir}/interfaces/filter

noinst_LTLIBRARIES = libaspell-static-filter.la

libaspell_static_filter_la_SOURCES = url.cpp 

CXXFLAGS = @CXXFLAGS@ -DFILTERS_STATIC_BUILD

optdir=${pkgdatadir}
noinst_DATA = url.opt

${noinst_DATA}: ${libaspell_static_filter_la_SOURCES} 
	${PERLPROG} ${top_srcdir}/scripts/mk-filter-opt.pl ${LIST_FILTER} $^
