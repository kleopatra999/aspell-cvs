export SED=@SED@
export PERLPROG=@PERLPROG@
filterdir = ${pkglibdir}
INCLUDES = -I${top_srcdir}/common -I${top_srcdir}/interfaces/filter
CXXFLAGS= @CXXFLAGS@
SUFFIXES = .opt

filter_LTLIBRARIES = libemail-filter.la libtex-filter.la\
		     libsgml-filter.la

libemail_filter_la_SOURCES = email.cpp

libemail_filter_la_LIBADD = ${top_builddir}/lib/libaspell.la

libtex_filter_la_SOURCES = tex.cpp

libtex_filter_la_LIBADD = ${top_builddir}/lib/libaspell.la

libsgml_filter_la_SOURCES  = sgml.cpp

libsgml_filter_la_LIBADD = ${top_builddir}/lib/libaspell.la


optdir=${pkgdatadir}
opt_DATA = email-filter.opt tex-filter.opt latex-filter.opt sgml-filter.opt

${opt_DATA}: generate_opt 

generate_opt: ${libemail_filter_la_SOURCES} ${libtex_filter_la_SOURCES} \
	      ${libsgml_filter_la_SOURCES}
	${PERLPROG} ${top_srcdir}/scripts/mk-filter-opt.pl ${LIST_FILTER} $^
	 
