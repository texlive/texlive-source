## Makefile.am for the TeX Live subdirectory libs/teckit/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## We want to re-distribute the whole original TECkit source tree.
##
EXTRA_DIST = $(TECKIT_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += $(TECKIT_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . include/teckit

AM_CPPFLAGS = -I$(top_srcdir)/$(TECKIT_TREE)/source/Public-headers $(ZLIB_INCLUDES)
AM_CPPFLAGS += -DNDEBUG
AM_CFLAGS = $(WARNING_CFLAGS)
AM_CXXFLAGS = $(WARNING_CXXFLAGS)

bin_PROGRAMS = teckit_compile

noinst_LIBRARIES = libTECkit_Compiler.a libTECkit.a

# teckit_compile
#
nodist_teckit_compile_SOURCES = \
	@TECKIT_TREE@/source/Sample-tools/TECkit_Compile.cpp

teckit_compile_LDADD = libTECkit_Compiler.a $(ZLIB_LIBS)

$(teckit_compile_OBJECTS): libTECkit_Compiler.a

# libTECkit_Compiler.a and libTECkit.a
#
nodist_libTECkit_Compiler_a_SOURCES = \
	@TECKIT_TREE@/source/Compiler.cpp \
	@TECKIT_TREE@/source/UnicodeNames.cpp

nodist_libTECkit_a_SOURCES = \
	@TECKIT_TREE@/source/Engine.cpp

$(libTECkit_Compiler_a_OBJECTS) $(libTECkit_a_OBJECTS): config.force

DISTCLEANFILES = CXXLD.sh

## Tests
##
if build
TESTS = teckit.test
endif build
teckit.log: teckit_compile$(EXEEXT)

EXTRA_DIST += \
	teckit.test \
	tex-text.map \
	tex-text.tec

## Files generated by TESTS
##
CLEANFILES = xtex-text.tec

## Rebuild zlib
@ZLIB_RULE@

# Reconfig
reconfig_prereq = $(ZLIB_DEPEND)

include $(srcdir)/../../am/reconfig.am

# Rebuild
rebuild_prereq =
rebuild_target = all

include $(srcdir)/../../am/rebuild.am

