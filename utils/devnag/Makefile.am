## Makefile.am for the TeX Live subdirectory utils/devnag/
##
## Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = -DTEXLIVE

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = devnag

devnag_SOURCES = src/devnag.c

$(devnag_OBJECTS): config.force

config.force: $(KPATHSEA_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

## Tests.
##
TESTS = tests/devnag.test
EXTRA_DIST = $(TESTS)
tests/devnag.log: devnag$(EXEEXT)
## tests/devnag.test
EXTRA_DIST += \
	tests/captions.dn tests/captions.tex \
	tests/examples.dn tests/examples.tex \
	tests/misspaal.dn tests/misspaal.tex \
	tests/vedasample.dn tests/vedasample.tex

DISTCLEANFILES += captions.dn examples.dn *.tex

