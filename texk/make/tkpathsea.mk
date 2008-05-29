# tkpathsea.mk -- target for remaking kpathsea.
# $Id$
makeargs = $(MFLAGS) CC='$(CC)' CFLAGS='$(CFLAGS)' $(XMAKEARGS)

# This is wrong: the library doesn't depend on kpsewhich.c or
# acconfig.h.  But what to do?
$(kpathsea): $(kpathsea_srcdir)/*.c $(kpathsea_srcdir)/*.h \
	     $(top_srcdir)/../make/paths.mk
	cd $(kpathsea_dir) && $(MAKE) $(makeargs)

$(kpathsea_dir)/paths.h: $(kpathsea_srcdir)/texmf.cnf
	cd $(kpathsea_dir) && $(MAKE) $(makeargs)
# End of tkpathsea.mk.
