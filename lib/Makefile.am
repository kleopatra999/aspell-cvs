#export SED=@SED@
export PERLPROG=@PERLPROG@

INCLUDES = -I${top_srcdir}/interfaces/cc/ -I${top_srcdir}/common

lib_LTLIBRARIES = libaspell.la libpspell.la

libaspell_la_SOURCES =\
  can_have_error-c.cpp  info-c.cpp              string_list-c.cpp\
  config-c.cpp          speller-c.cpp           string_map-c.cpp\
  error-c.cpp           mutable_container-c.cpp string_pair_enumeration-c.cpp\
  find_speller.cpp      new_checker.cpp 	static_filters.cpp new_filter.cpp\
  new_config.cpp	string_enumeration-c.cpp  word_list-c.cpp \
  filter-c.cpp          document_checker-c.cpp	filter_entry.hpp

if FORCE_STATIC

libaspell_la_LIBADD = ${top_builddir}/common/libaspell-common.la \
                      ${top_builddir}/modules/speller/default/libaspell-speller-default.la \
                      ${top_builddir}/modules/tokenizer/libaspell-tokenizer-standard.la \
                      ${top_builddir}/modules/filter/libaspell-static-filter.la \
		      ${top_builddir}/runtime-modules/filter/libaspell-loadable-filter.la

else
libaspell_la_LIBADD = ${top_builddir}/common/libaspell-common.la \
                      ${top_builddir}/modules/speller/default/libaspell-speller-default.la \
                      ${top_builddir}/modules/tokenizer/libaspell-tokenizer-standard.la \
                      ${top_builddir}/modules/filter/libaspell-static-filter.la 


endif
libaspell_la_LDFLAGS = ${extra_ldflags} -version-info 15:2:0

libpspell_la_SOURCES = dummy.cpp

libpspell_la_LIBADD = libaspell.la

libpspell_la_LDFLAGS = ${extra_ldflags} -version-info 15:2:0

if FORCE_STATIC
STATIC_FILTERS_OPTION_FILES := $(shell ls ${top_builddir}/modules/filter/*.opt ${top_builddir}/runtime-modules/filter/*.opt 2>/dev/null)
else
STATIC_FILTERS_OPTION_FILES := $(shell ls ${top_builddir}/modules/filter/*.opt 2>/dev/null)
endif

static_filters.cpp: ${STATIC_FILTERS_OPTION_FILES} FORCE
	${PERLPROG} ${top_srcdir}/scripts/mk-static-filter.pl ${STATIC_FILTERS_OPTION_FILES}

FORCE:
