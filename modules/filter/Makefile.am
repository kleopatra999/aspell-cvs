#export SED = @SED@
INCLUDES = -I${top_srcdir}/common -I${top_srcdir}/interfaces/filter

########MODIFY HERE BEGIN########
#add here all source files .hpp and .cpp files which are part of your filter
noinst_LTLIBRARIES = libaspell-static-filter.la
#########MODIFY HERE END#########

libaspell_static_filter_la_SOURCES = url.cpp 

CXXFLAGS = @CXXFLAGS@ -DFILTERS_STATIC_BUILD

optdir=${pkgdatadir}

########MODIFY HERE BEGIN########
#add here the corresponding .opt file your filter comes with.
noinst_DATA = url.opt
#########MODIFY HERE END#########

${noinst_DATA}: ${libaspell_static_filter_la_SOURCES} 
	${PERLPROG} ${top_srcdir}/scripts/mk-filter-opt.pl ${LIST_FILTER} $^
