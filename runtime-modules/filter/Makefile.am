#export SED=@SED@
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

########MODIFY HERE BEGIN########
#Add your filter sources here, starting with file containing filter class definition
#followed by file containing filter member implementation.
libaspell_loadable_filter_la_SOURCES = email.cpp tex.cpp sgml.cpp\
				 ccpp-context.hpp ccpp-context.cpp
#########MODIFY HERE END#########
optdir=${pkgdatadir}

########MODIFY HERE BEGIN########
#add the .opt file your filter comes with
noinst_DATA = email-filter.opt tex-filter.opt latex-filter.opt sgml-filter.opt
#########MODIFY HERE END#########

########MODIFY HERE BEGIN########
#if your filter sources contain ALIAS line add .flt file corresponding to entire alias name
filter_DATA = latex.flt
#########MODIFY HERE END#########

${noinst_DATA}: ${libaspell_loadable_filter_la_SOURCES}
	${PERLPROG} ${top_srcdir}/scripts/mk-filter-opt.pl ${LIST_FILTER} $^

else


########MODIFY HERE BEGIN########
#add name of filter library containing your filter
#name always must look like lib<filtername>-filter.la see development manual
filter_LTLIBRARIES = libemail-filter.la libtex-filter.la\
	  	       libsgml-filter.la libcontext-filter.la
#########MODIFY HERE END#########

libemail_filter_la_SOURCES = email.cpp

libemail_filter_la_LIBADD = ${top_builddir}/lib/libaspell.la

libtex_filter_la_SOURCES = tex.cpp

libtex_filter_la_LIBADD = ${top_builddir}/lib/libaspell.la

libsgml_filter_la_SOURCES  = sgml.cpp

libsgml_filter_la_LIBADD = ${top_builddir}/lib/libaspell.la

libcontext_filter_la_SOURCES  = ccpp-context.hpp ccpp-context.cpp

libcontext_filter_la_LIBADD = ${top_builddir}/lib/libaspell.la

########MODIFY HERE BEGIN########
#add here the corresponding <yourfilterlibrary>_SOURCES and
#                           <yourfilterlibrary>_LIBADD lines
# the later at least has to look like
# <yourfilterlibrary>_LIBADD = ${top_builddir}/lib/libaspell.la
# in order to make your filter build properly



#########MODIFY HERE END#########

optdir=${pkgdatadir}

########MODIFY HERE BEGIN########
#add the .opt file your filter comes with
opt_DATA = email-filter.opt tex-filter.opt context-filter.opt sgml-filter.opt
#########MODIFY HERE END#########

########MODIFY HERE BEGIN########
#if your filter sources contain ALIAS line add .flt file corresponding to entire alias name
filter_DATA = latex.flt
#########MODIFY HERE END#########


########MODIFY HERE BEGIN########
#finally expand the target line in order to make the ${opt_DATA} target dependent upon
#your filter sources expand the ${<filterlibrary>_SOURCES} list by the corresponding variable
#belonging to your filter. Do not change any thing else.
${opt_DATA}: ${libemail_filter_la_SOURCES} ${libtex_filter_la_SOURCES} \
	     ${libsgml_filter_la_SOURCES} ${libcontext_filter_la_SOURCES}
	${PERLPROG} ${top_srcdir}/scripts/mk-filter-opt.pl ${LIST_FILTER} $^
#########MODIFY HERE END#########

endif
