/*
 *  Code to output PostScript commands for one section of the document.
 */
#include "dvips.h" /* The copyright notice in that file is included too! */
/*
 *   The external declarations:
 */
#include "protos.h"

static int psfont;
#ifdef HPS
int pagecounter;
#endif

/*
 *   Now we have the main procedure.
 */
void
dosection(sectiontype *s, int c)
{
   charusetype *cu;
   integer prevptr;
   int np;
   int k;
   integer thispage = 0;
   char buf[104];

   dopsfont(s);
#ifdef HPS
	 if (HPS_FLAG) pagecounter = 0;
#endif

   if (multiplesects) {
     setup();
   }
   cmdout("TeXDict");
   cmdout("begin");
   numout(hpapersize);
   numout(vpapersize);
   doubleout(mag);
   numout((integer)DPI);
   numout((integer)VDPI);
   snprintf(buf, sizeof(buf), "(%.99s)", fulliname);
   cmdout(buf);
   newline();
   cmdout("@start");
   if (multiplesects)
      cmdout("bos");
/*
 *   We insure raster is even-word aligned, because download might want that.
 */
   if (bytesleft & 1) {
      bytesleft--;
      raster++;
   }
   cleanres();
   cu = (charusetype *) (s + 1);
   psfont = 1;
   while (cu->fd) {
      if (cu->psfused)
         cu->fd->psflag = EXISTS;
      download(cu++, psfont++);
   }
   fonttableout();
   if (! multiplesects) {
      cmdout("end");
      setup();
   }
   for (cu=(charusetype *)(s+1); cu->fd; cu++)
      cu->fd->psflag = 0;
   while (c > 0) {
      c--;
      prevptr = s->bos;
      if (! reverse)
         fseek(dvifile, (long)prevptr, 0);
      np = s->numpages;
      while (np-- != 0) {
         if (reverse)
            fseek(dvifile, (long)prevptr, 0);
         pagenum = signedquad();
	 if ((evenpages && (pagenum & 1)) || (oddpages && (pagenum & 1)==0) ||
	  (pagelist && !InPageList(pagenum))) {
	    if (reverse) {
               skipover(36);
               prevptr = signedquad()+1;
	    } else {
               skipover(40);
	       skippage();
	       skipnop();
	    }
	    ++np;	/* this page wasn't counted for s->numpages */
	    continue;
	 }
/*
 *   We want to take the base 10 log of the number.  It's probably
 *   small, so we do it quick.
 */
         if (! quiet) {
            int t = pagenum, i = 0;
            if (t < 0) {
               t = -t;
               i++;
            }
            do {
               i++;
               t /= 10;
            } while (t > 0);
            if (pagecopies < 20)
               i += pagecopies - 1;
            if (i + prettycolumn > STDOUTSIZE) {
               fprintf(stderr, "\n");
               prettycolumn = 0;
            }
            prettycolumn += i + 1;
#ifdef SHORTINT
            fprintf(stderr, "[%ld", pagenum);
#else  /* ~SHORTINT */
            fprintf(stderr, "[%d", pagenum);
#endif /* ~SHORTINT */
            fflush(stderr);
         }
         skipover(36);
         prevptr = signedquad()+1;
         for (k=0; k<pagecopies; k++) {
            if (k == 0) {
               if (pagecopies > 1)
                  thispage = ftell(dvifile);
            } else {
               fseek(dvifile, (long)thispage, 0);
               if (prettycolumn + 1 > STDOUTSIZE) {
                  fprintf(stderr, "\n");
                  prettycolumn = 0;
               }
               fprintf(stderr, ".");
               fflush(stderr);
               prettycolumn++;
            }
            dopage();
         }
         if (! quiet) {
            fprintf(stderr, "] ");
            fflush(stderr);
            prettycolumn += 2;
         }
         if (! reverse)
            skipnop();
      }
   }
   if (! multiplesects && ! disablecomments) {
      newline();
      fprintf(bitfile, "%%%%Trailer\n");
   }
   if (multiplesects) {
      if (! disablecomments) {
         newline();
         fprintf(bitfile, "%%DVIPSSectionTrailer\n");
      }
      cmdout("eos");
      cmdout("end");
   }
#ifdef HPS
   if (HPS_FLAG) cmdout("\nend"); /* close off HPSDict */
#endif
   if (multiplesects && ! disablecomments) {
      newline();
      fprintf(bitfile, "%%DVIPSEndSection\n");
      linepos = 0;
   }
}
/*
 * Handle a list of pages for dvips.  Code based on dvi2ps 2.49,
 * maintained by Piet van Oostrum, piet@cs.ruu.nl.  Collected and
 * modularized for inclusion in dvips by metcalf@lcs.mit.edu.
 */

#include <ctype.h>
#define MAXPAGE (1000000000) /* assume no pages out of this range */
struct p_list_str {
    struct p_list_str *next;	/* next in a series of alternates */
    integer ps_low, ps_high;	/* allowed range */
} *ppages = 0;	/* the list of allowed pages */

/*-->InPageList*/
/**********************************************************************/
/******************************  InPageList  **************************/
/**********************************************************************/
/* Return true iff i is one of the desired output pages */

int
InPageList(integer i)
{
    register struct p_list_str *pl = ppages;

    while (pl) {
	    if ( i >= pl -> ps_low && i <= pl -> ps_high)
		return 1;		/* success */
	pl = pl -> next;
    }
    return 0;
}

static void
InstallPL(integer pslow, integer pshigh)
{
    register struct p_list_str   *pl;

    pl = (struct p_list_str *)mymalloc((integer)(sizeof *pl));
    pl -> next = ppages;
    pl -> ps_low = pslow;
    pl -> ps_high = pshigh;
    ppages = pl;
}

/* Parse a string representing a list of pages.  Return 0 iff ok.  As a
   side effect, the page selection(s) is (are) prepended to ppages. */

int
ParsePages(register char *s)
{
    register int    c;		/* current character */
    register integer  n = 0,	/* current numeric value */
		    innumber;	/* true => gathering a number */
    integer ps_low = 0, ps_high = 0;
    int     range,		/* true => saw a range indicator */
	    negative = 0;	/* true => number being built is negative */

#define white(x) ((x) == ' ' || (x) == '\t' || (x) == ',')

    range = 0;
    innumber = 0;
    for (;;) {
	c = *s++;
	if ( !innumber && !range) {/* nothing special going on */
	    if (c == 0)
		return 0;
	    if (white (c))
		continue;
	}
	if (c == '-' && !innumber) {
		innumber++;
		negative++;
		n = 0;
		continue;
	}
	if ('0' <= c && c <= '9') {	/* accumulate numeric value */
	    if (!innumber) {
		innumber++;
		negative = 0;
		n = c - '0';
		continue;
	    }
	    n *= 10;
	    n += negative ? '0' - c : c - '0';
	    continue;
	}
	if (c == '-' || c == ':') {/* here's a range */
	    if (range)
		return (-1);
	    if (innumber) {	/* have a lower bound */
		ps_low = n;
	    }
	    else
		ps_low = -MAXPAGE;
	    range++;
	    innumber = 0;
	    continue;
	}
	if (c == 0 || white (c)) {/* end of this range */
	    if (!innumber) {	/* no upper bound */
		ps_high = MAXPAGE;
		if (!range)	/* no lower bound either */
		    ps_low = -MAXPAGE;
	    }
	    else {		/* have an upper bound */
		ps_high = n;
		if (!range) {	/* no range => lower bound == upper */
		    ps_low = ps_high;
		}
	    }
	    InstallPL (ps_low, ps_high);
	    if (c == 0)
		return 0;
	    range = 0;
	    innumber = 0;
	    continue;
	}
	return (-1);
    }
#undef white
}
