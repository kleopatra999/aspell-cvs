export SED=@SED@
export PERLPROG=@PERLPROG@
filterdir = ${pkglibdir}
INCLUDES = -I${top_srcdir}/common -I${top_srcdir}/interfaces/filter
SUFFIXES = .opt
if FORCE_STATIC
MY_CXXFLAGS = -DFILTERS_STATIC_BUILD   -DVERSION="\"${FILTERVERSION}\""
else
MY_CXXFLAGS =  -DVERSION="\"${FILTERVERSION}\""
endif

CXXFLAGS = @CXXFLAGS@ ${MY_CXXFLAGS}

if FORCE_STATIC 

noinst_LTLIBRARIES = libaspell-loadable-filter.la
libaspell_loadable_filter_la_SOURCES = email.cpp tex.cpp sgml.cpp\
				 ccpp-context.hpp ccpp-context.cpp
optdir=${pkgdatadir}
noinst_DATA = email-filter.opt tex-filter.opt latex-filter.opt sgml-filter.opt
filter_DATA = latex.flt

${noinst_DATA}: ${libaspell_loadable_filter_la_SOURCES}
	${PERLPROG} ${top_srcdir}/scripts/mk-filter-opt.pl ${LIST_FILTER} $^

else
filter_LTLIBRARIES = libemail-filter.la libtex-filter.la\
	  	       libsgml-filter.la libcontext-filter.la

libemail_filter_la_SOURCES = email.cpp

libemail_filter_la_LIBADD = ${top_builddir}/lib/libaspell.la

libtex_filter_la_SOURCES = tex.cpp

libtex_filter_la_LIBADD = ${top_builddir}/lib/libaspell.la

libsgml_filter_la_SOURCES  = sgml.cpp

libsgml_filter_la_LIBADD = ${top_builddir}/lib/libaspell.la

libcontext_filter_la_SOURCES  = ccpp-context.hpp ccpp-context.cpp

libcontext_filter_la_LIBADD = ${top_builddir}/lib/libaspell.la

optdir=${pkgdatadir}
opt_DATA = email-filter.opt tex-filter.opt context-filter.opt sgml-filter.opt
filter_DATA = latex.flt

${opt_DATA}: ${libemail_filter_la_SOURCES} ${libtex_filter_la_SOURCES} \
	     ${libsgml_filter_la_SOURCES} ${libcontext_filter_la_SOURCES}
	${PERLPROG} ${top_srcdir}/scripts/mk-filter-opt.pl ${LIST_FILTER} $^

endif
