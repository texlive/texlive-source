# git.mk
#
# Copyright 2009, Red Hat, Inc.
# Copyright 2010,2011 Behdad Esfahbod
# Written by Behdad Esfahbod
#
# Copying and distribution of this file, with or without modification,
# is permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.
#
# The canonical source for this file is https://github.com/behdad/git.mk.
#
# To use in your project, import this file in your git repo's toplevel,
# then do "make -f git.mk".  This modifies all Makefile.am files in
# your project to -include git.mk.  Remember to add that line to new
# Makefile.am files you create in your project, or just rerun the
# "make -f git.mk".
#
# This enables automatic .gitignore generation.  If you need to ignore
# more files, add them to the GITIGNOREFILES variable in your Makefile.am.
# But think twice before doing that.  If a file has to be in .gitignore,
# chances are very high that it's a generated file and should be in one
# of MOSTLYCLEANFILES, CLEANFILES, DISTCLEANFILES, or MAINTAINERCLEANFILES.
#
# The only case that you need to manually add a file to GITIGNOREFILES is
# when remove files in one of mostlyclean-local, clean-local, distclean-local,
# or maintainer-clean-local make targets.
#
# Note that for files like editor backup, etc, there are better places to
# ignore them.  See "man gitignore".
#
# If "make maintainer-clean" removes the files but they are not recognized
# by this script (that is, if "git status" shows untracked files still), send
# me the output of "git status" as well as your Makefile.am and Makefile for
# the directories involved and I'll diagnose.
#
# For a list of toplevel files that should be in MAINTAINERCLEANFILES, see
# Makefile.am.sample in the git.mk git repo.
#
# Don't EXTRA_DIST this file.  It is supposed to only live in git clones,
# not tarballs.  It serves no useful purpose in tarballs and clutters the
# build dir.
#
# This file knows how to handle autoconf, automake, libtool, gtk-doc,
# gnome-doc-utils, yelp.m4, mallard, intltool, gsettings, dejagnu.
#
# This makefile provides the following targets:
#
# - all: "make all" will build all gitignore files.
# - gitignore: makes all gitignore files in the current dir and subdirs.
# - .gitignore: make gitignore file for the current dir.
# - gitignore-recurse: makes all gitignore files in the subdirs.
#
# KNOWN ISSUES:
#
# - Recursive configure doesn't work as $(top_srcdir)/git.mk inside the
#   submodule doesn't find us.  If you have configure.{in,ac} files in
#   subdirs, add a proxy git.mk file in those dirs that simply does:
#   "include $(top_srcdir)/../git.mk".  Add more ..'s to your taste.
#   And add those files to git.  See vte/gnome-pty-helper/git.mk for
#   example.
#

git-all: git-mk-install

git-mk-install:
	@echo Installing git makefile
	@any_failed=; \
		find "`test -z "$(top_srcdir)" && echo . || echo "$(top_srcdir)"`" -name Makefile.am | while read x; do \
		if grep 'include .*/git.mk' $$x >/dev/null; then \
			echo $$x already includes git.mk; \
		else \
			failed=; \
			echo "Updating $$x"; \
			{ cat $$x; \
			  echo ''; \
			  echo '-include $$(top_srcdir)/git.mk'; \
			} > $$x.tmp || failed=1; \
			if test x$$failed = x; then \
				mv $$x.tmp $$x || failed=1; \
			fi; \
			if test x$$failed = x; then : else \
				echo Failed updating $$x; >&2 \
				any_failed=1; \
			fi; \
	fi; done; test -z "$$any_failed"

.PHONY: git-all git-mk-install


### .gitignore generation

$(srcdir)/.gitignore: Makefile.am $(top_srcdir)/git.mk
	$(AM_V_GEN) \
	{ \
		if test "x$(DOC_MODULE)" = x -o "x$(DOC_MAIN_SGML_FILE)" = x; then :; else \
			for x in \
				$(DOC_MODULE)-decl-list.txt \
				$(DOC_MODULE)-decl.txt \
				tmpl/$(DOC_MODULE)-unused.sgml \
				"tmpl/*.bak" \
				xml html \
			; do echo /$$x; done; \
		fi; \
		if test "x$(DOC_MODULE)$(DOC_ID)" = x -o "x$(DOC_LINGUAS)" = x; then :; else \
			for lc in $(DOC_LINGUAS); do \
				for x in \
					$(if $(DOC_MODULE),$(DOC_MODULE).xml) \
					$(DOC_PAGES) \
					$(DOC_INCLUDES) \
				; do echo /$$lc/$$x; done; \
			done; \
			for x in \
				$(_DOC_OMF_ALL) \
				$(_DOC_DSK_ALL) \
				$(_DOC_HTML_ALL) \
				$(_DOC_MOFILES) \
				$(DOC_H_FILE) \
				"*/.xml2po.mo" \
				"*/*.omf.out" \
			; do echo /$$x; done; \
		fi; \
		if test "x$(HELP_ID)" = x -o "x$(HELP_LINGUAS)" = x; then :; else \
			for lc in $(HELP_LINGUAS); do \
				for x in \
					$(HELP_FILES) \
					"$$lc.stamp" \
					"$$lc.mo" \
				; do echo /$$lc/$$x; done; \
			done; \
		fi; \
		if test "x$(gsettings_SCHEMAS)" = x; then :; else \
			for x in \
				$(gsettings_SCHEMAS:.xml=.valid) \
				$(gsettings__enum_file) \
			; do echo /$$x; done; \
		fi; \
		if test -f $(srcdir)/po/Makefile.in.in; then \
			for x in \
				po/Makefile.in.in \
				po/Makefile.in \
				po/Makefile \
				po/POTFILES \
				po/stamp-it \
				po/.intltool-merge-cache \
				"po/*.gmo" \
				"po/*.mo" \
				po/$(GETTEXT_PACKAGE).pot \
				intltool-extract.in \
				intltool-merge.in \
				intltool-update.in \
			; do echo /$$x; done; \
		fi; \
		if test -f $(srcdir)/configure; then \
			for x in \
				autom4te.cache \
				configure \
				config.h \
				stamp-h1 \
				libtool \
				config.lt \
			; do echo /$$x; done; \
		fi; \
		if test "x$(DEJATOOL)" = x; then :; else \
			for x in \
				$(DEJATOOL) \
			; do echo /$$x.sum; echo /$$x.log; done; \
			echo /site.exp; \
		fi; \
		for x in \
			.gitignore \
			$(GITIGNOREFILES) \
			$(CLEANFILES) \
			$(PROGRAMS) $(check_PROGRAMS) $(EXTRA_PROGRAMS) \
			$(LIBRARIES) $(check_LIBRARIES) $(EXTRA_LIBRARIES) \
			$(LTLIBRARIES) $(check_LTLIBRARIES) $(EXTRA_LTLIBRARIES) \
			so_locations \
			.libs _libs \
			$(MOSTLYCLEANFILES) \
			"*.$(OBJEXT)" \
			"*.lo" \
			$(DISTCLEANFILES) \
			$(am__CONFIG_DISTCLEAN_FILES) \
			$(CONFIG_CLEAN_FILES) \
			TAGS ID GTAGS GRTAGS GSYMS GPATH tags \
			"*.tab.c" \
			$(MAINTAINERCLEANFILES) \
			$(BUILT_SOURCES) \
			$(DEPDIR) \
			Makefile \
			Makefile.in \
			"*.orig" \
			"*.rej" \
			"*.bak" \
			"*~" \
			".*.sw[nop]" \
			".dirstamp" \
		; do echo /$$x; done; \
	} | \
	sed "s@^/`echo "$(srcdir)" | sed 's/\(.\)/[\1]/g'`/@/@" | \
	sed 's@/[.]/@/@g' | \
	LC_ALL=C sort | uniq > $@.tmp && \
	mv $@.tmp $@;

all: $(srcdir)/.gitignore gitignore-recurse-maybe
gitignore: $(srcdir)/.gitignore gitignore-recurse

gitignore-recurse-maybe:
	@for subdir in $(DIST_SUBDIRS); do \
	  case " $(SUBDIRS) " in \
	    *" $$subdir "*) :;; \
	    *) test "$$subdir" = . || (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) .gitignore gitignore-recurse-maybe || echo "Skipping $$subdir");; \
	  esac; \
	done
gitignore-recurse:
	@for subdir in $(DIST_SUBDIRS); do \
	    test "$$subdir" = . || (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) .gitignore gitignore-recurse || echo "Skipping $$subdir"); \
	done

maintainer-clean: gitignore-clean
gitignore-clean:
	-rm -f $(srcdir)/.gitignore

.PHONY: gitignore-clean gitignore gitignore-recurse gitignore-recurse-maybe
