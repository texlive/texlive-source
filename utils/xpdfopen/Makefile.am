## Makefile.am for the TeX Live subdirectory utils/xpdfopen/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
EXTRA_DIST = $(XPDFOPEN_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(XPDFOPEN_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CPPFLAGS = $(X_CFLAGS)
AM_CFLAGS = $(WARNING_CFLAGS)

LDADD = $(X_LIBS) -lX11 -lXmu $(X_EXTRA_LIBS)

bin_PROGRAMS = pdfopen pdfclose

common_sources = \
	@XPDFOPEN_TREE@/sendx.c \
	@XPDFOPEN_TREE@/utils.c

nodist_pdfopen_SOURCES = \
	@XPDFOPEN_TREE@/pdfopen.c \
	$(common_sources)

nodist_pdfclose_SOURCES = \
	@XPDFOPEN_TREE@/pdfclose.c \
	$(common_sources)

nodist_man1_MANS = $(XPDFOPEN_TREE)/pdfopen.1

# Symlinks within $(man1dir): FILE:LINK indicates LINK.1->FILE.1
man1_links = pdfopen:pdfclose

include $(top_srcdir)/../../am/man1_links.am

install-data-hook: install-man1-links

uninstall-hook: uninstall-man1-links

EXTRA_DIST += xpdfopen.pdf xpdfopen.tex

