export SED=@SED@

pkgdoc_DATA = manual.dvi manual.tex manual2.lyx manual.aux\
              devel.dvi  devel.tex devel2.lyx devel.aux
noinst_DATA = manual-html manual-text devel-html devel-text

## CLEANFILES += manual2.tex manual2.aux manual2.log

MAINTAINERCLEANFILES = manual.*  manual-html manual-text\
                       devel.*  devel-html devel-text

export TEXT_DUMP=@TEXT_DUMP@
export DUMP_WIDTH=@DUMP_WIDTH@

fake-manual:
	touch \
	manual.dvi manual.tex manual.aux manual2.tex manual-html manual-text\
	devel.dvi devel.tex devel.aux devel2.tex devel-html devel-text

#
# Main Manual
#

manual: @MAINTAINER_MODE_TRUE@ manual-text devel-text

manual2.tex: @MAINTAINER_MODE_TRUE@ manual2.lyx
	-lyx --export latex manual2.lyx

manual.tex: @MAINTAINER_MODE_TRUE@ man-proc.pl manual2.tex
	perl man-proc.pl manual

manual.dvi: @MAINTAINER_MODE_TRUE@ manual.tex
	latex manual.tex
	latex manual.tex
	latex manual.tex

manual-html: @MAINTAINER_MODE_TRUE@ manual.dvi
	if test -d man-html;then\
	  cd man-html;\
	  rm -f *.html *.css *.png;\
	  cd ..;\
	else \
	  mkdir man-html;\
	fi;
	latex2html \
	  -split 3 \
          -toc_depth 3 \
	  -dir man-html \
	  -long_titles 2 \
	  -local_icons \
	  -show_section_numbers \
	  manual.tex
	chmod -R ugo+rX man-html/
	touch manual-html

manual-text: @MAINTAINER_MODE_TRUE@ manual-html html2txt.pl
	if test -d man-text;then\
	  cd man-text;\
	  rm -f *.txt;\
	  cd ..;\
	else \
	  mkdir man-text;\
	fi;
	perl html2txt.pl man
	touch manual-text

manual.ps: @MAINTAINER_MODE_TRUE@ manual.dvi
	dvips -o manual.ps -t letter manual.dvi

#
# Devel Docs
#

devel: @MAINTAINER_MODE_TRUE@ devel-text

devel2.tex: @MAINTAINER_MODE_TRUE@ devel2.lyx
	-lyx --export latex devel2.lyx

devel.tex: @MAINTAINER_MODE_TRUE@ man-proc.pl devel2.tex mk-src.tex
	perl man-proc.pl devel

devel.dvi: @MAINTAINER_MODE_TRUE@ devel.tex
	latex devel.tex
	latex devel.tex
	latex devel.tex

devel-html: @MAINTAINER_MODE_TRUE@ devel.dvi
	if test -d dev-html;then\
	  cd dev-html;\
	  rm -f *.html *.css *.png;\
	  cd ..;\
	else \
	  mkdir dev-html;\
	fi;
	latex2html \
	  -split 3 \
          -toc_depth 3 \
	  -dir dev-html \
	  -long_titles 2 \
	  -local_icons \
	  -show_section_numbers \
	  devel.tex
	chmod -R ugo+rX dev-html/
	touch devel-html

devel-text: @MAINTAINER_MODE_TRUE@ devel-html html2txt.pl
	if test -d dev-text;then\
	  cd dev-text;\
	  rm -f *.txt;\
	  cd ..;\
	else \
	  mkdir dev-text;\
	fi;
	perl html2txt.pl dev
	touch devel-text

devel.ps: @MAINTAINER_MODE_TRUE@ devel.dvi
	dvips -o devel.ps -t letter devel.dvi
#
#
#

#SUBDIRS = man-html man-text dev-html dev-text

EXTRA_DIST = ${pkgdoc_DATA} \
             html.sty html2txt.pl edit-man manual2.per man-proc.pl\
	     manual-text manual-html devel-html devel-text


