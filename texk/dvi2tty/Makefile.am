## Makefile.am for the TeX Live subdirectory texk/dvi2tty/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## We want to re-distribute the whole original dvi2tty source tree.
##
EXTRA_DIST = $(DVI2TTY_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += $(DVI2TTY_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CPPFLAGS = $(PTEXENC_INCLUDES) $(KPATHSEA_INCLUDES) -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

LDADD = $(KPATHSEA_LIBS)

bin_PROGRAMS = disdvi dvi2tty

disdvi_SOURCES = \
	@DVI2TTY_TREE@/commands.h \
	@DVI2TTY_TREE@/disdvi.c

$(disdvi_OBJECTS): $(KPATHSEA_DEPEND)

dvi2tty_SOURCES = \
	@DVI2TTY_TREE@/commands.h \
	@DVI2TTY_TREE@/dvi2tty.c \
	@DVI2TTY_TREE@/dvi2tty.h \
	@DVI2TTY_TREE@/dvistuff.c

$(dvi2tty_OBJECTS): $(KPATHSEA_DEPEND) $(PTEXENC_DEPEND)

dvi2tty_LDADD = $(PTEXENC_LIBS) $(LDADD)

man1_MANS = \
	$(DVI2TTY_TREE)/disdvi.1 \
	$(DVI2TTY_TREE)/dvi2tty.1

## Rebuild libkpathsea & libptexenc
@KPATHSEA_RULE@
@PTEXENC_RULE@

## Tests.
##
TESTS = disdvi.test dvi2tty.test
disdvi.log: disdvi$(EXEXET)
dvi2tty.log: dvi2tty$(EXEEXT)

TESTS_ENVIRONMENT = DVI2TTY_TREE=$(DVI2TTY_TREE)
EXTRA_DIST += $(TESTS)

## disdvi.test
EXTRA_DIST += ligtest.dis
DISTCLEANFILES = ligtest.dout
## dvi2tty.test
EXTRA_DIST += ligtest.tty test.tty testc.tty tests.tty testu.tty
DISTCLEANFILES += ligtest.out test.out testc.out tests.out testu.out

