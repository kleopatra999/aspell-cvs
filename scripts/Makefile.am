#export SED=@SED@
export PERLPROG=@PERLPROG@

pkgdatadir = @pkgdatadir@

bin_SCRIPTS = pspell-config run-with-aspell aspell-import mk-flt-opt.pl
pkgdata_SCRIPTS = spell ispell

run-with-aspell: run-with-aspell.create
	sh ${srcdir}/run-with-aspell.create ${pkgdatadir} > run-with-aspell
	chmod 755 run-with-aspell

pspell-config: mkconfig
	sh ${srcdir}/mkconfig ${VERSION} ${datadir} ${pkgdatadir}

mk-flt-opt.pl: mk-filter-opt.pl
	echo "#!${PERLPROG}" > mk-flt-opt.pl
	cat ${srcdir}/mk-filter-opt.pl >> mk-flt-opt.pl
	chmod 755 mk-flt-opt.pl

CLEANFILES = pspell-config run-with-aspell mk-flt-opt.pl

EXTRA_DIST = mkconfig spell ispell run-with-aspell.create aspell-import mk-filter-opt.pl

