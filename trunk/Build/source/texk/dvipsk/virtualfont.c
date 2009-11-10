/*
 *   Here's the code to load a VF file into memory.
 *   Any resemblance between this file and loadfont.c is purely uncoincidental.
 */
#include "dvips.h" /* The copyright notice in that file is included too! */
#ifdef KPATHSEA
#include <kpathsea/c-pathmx.h>
#endif
/*
 *   The external declarations:
 */
#include "protos.h"
/*
 *   Now we have some routines to get stuff from the VF file.
 *   Subroutine vfbyte returns the next byte.
 */
static FILE *vffile ;
static char name[50] ;
void
badvf(const char *s)
{
   (void)sprintf(errbuf,"! Bad VF file %s: %s",name,s) ;
   error(errbuf);
}

shalfword
vfbyte(void)
{
   register shalfword i ;

   if ((i=getc(vffile))==EOF)
      badvf("unexpected eof") ;
   return(i) ;
}

integer
vfquad(void)
{
   register integer i ;

   i = vfbyte() ;
   if (i > 127)
      i -= 256 ;
   i = i * 256 + vfbyte() ;
   i = i * 256 + vfbyte() ;
   i = i * 256 + vfbyte() ;
   return(i) ;
}

integer
vftrio(void)
{
   register integer i ;

   i = vfbyte() ;
   i = i * 256 + vfbyte() ;
   i = i * 256 + vfbyte() ;
   return(i) ;
}

int
vfopen(register fontdesctype *fd)
{
   register char *n ;
#ifndef KPATHSEA
   register char *d ;
#endif

   n = fd->name ;
#ifndef KPATHSEA
   d = fd->area ;
   if (*d==0)
      d = vfpath ;
#endif
#ifdef MVSXA   /* IBM: MVS/XA */
   (void)sprintf(name, "vf(%s)", n) ;
#else
   (void)sprintf(name, "%s.vf", n) ;
#endif
#ifdef KPATHSEA
   if (0 != (vffile=search(vfpath, name, READBIN)))
#else
   if (0 != (vffile=search(d, name, READBIN)))
#endif
      return(1) ;
   if (!noomega)
#ifdef KPATHSEA
      if (0 != (vffile=search(ovfpath, n, READBIN)))
#else
      if (0 != (vffile=search(d, n, READBIN)))
#endif
         return(2) ;
   return(0) ;
}

/*
 * The following routine is like fontdef, but for local fontdefs in VF files.
 */
fontmaptype *
vfontdef(integer s, int siz)
{
   register integer i, j, fn ;
   register fontdesctype *fp ;
   register fontmaptype *cfnt ;
   char *nam, *area ;
   integer cksum, scsize, dssize ;

   fn = vfbyte() ;
   while (siz-- > 1)
      fn = (fn << 8) + vfbyte() ;
   cfnt = (fontmaptype *)mymalloc((integer)sizeof(fontmaptype)) ;
   cfnt->fontnum = fn ;
   cksum = vfquad() ;
   scsize = scalewidth(s, vfquad()) ;
   dssize = (integer)(alpha * (real)vfquad()) ;
   i = vfbyte() ; j = vfbyte() ;
   if (nextstring + i + j > maxstring)
      error("! out of string space") ;
   area = nextstring ;
   for (; i>0; i--)
      *nextstring++ = vfbyte() ;
   *nextstring++ = 0 ;
   nam = nextstring ;
   for (; j>0; j--)
      *nextstring++ = vfbyte() ;
   *nextstring++ = 0 ;
   fp = matchfont(nam, area, scsize, (char *)0) ;
   if (fp) {
      nextstring = nam ;
      fp->checksum = cksum ;
   } else {
      fp = newfontdesc(cksum, scsize, dssize, nam, area) ;
      fp->next = fonthead ;
      fonthead = fp ;
   }
   cfnt->desc = fp ;
   return (cfnt) ;
}

/*
 *   Now our virtualfont routine.
 */
Boolean
virtualfont(register fontdesctype *curfnt)
{
   register integer i ;
   register shalfword cmd ;
   register integer k ;
   register integer length ;
   register integer cc ;
   register chardesctype *cd ;
   integer scaledsize = curfnt->scaledsize ;
   integer no_of_chars=256 ;
   integer maxcc=255 ;
   register quarterword *tempr ;
   fontmaptype *fm, *newf ;
   int kindfont ;
   kindfont = vfopen(curfnt) ;  /* 1 for TeX, 2 for Omega */
   if (!kindfont)
      return (0) ;
#ifdef DEBUG
   if (dd(D_FONTS))
      (void)fprintf(stderr,"Loading virtual font %s at %.1fpt\n",
         name, (real)scaledsize/(alpha*0x100000)) ;
#endif /* DEBUG */

/*
 *   We clear out some pointers:
 */
   if (kindfont==2) {
     no_of_chars = 65536;
     curfnt->maxchars=65536;
     free(curfnt->chardesc);
     curfnt->chardesc = (chardesctype *)
                        mymalloc(65536 * (integer)sizeof(chardesctype));
   }
   for (i=0; i<no_of_chars; i++) {
      curfnt->chardesc[i].TFMwidth = 0 ;
      curfnt->chardesc[i].packptr = NULL ;
      curfnt->chardesc[i].pixelwidth = 0 ;
      curfnt->chardesc[i].flags = 0 ;
      curfnt->chardesc[i].flags2 = 0 ;
   }
   if (vfbyte()!=247)
      badvf("expected pre") ;
   if (vfbyte()!=202)
      badvf("wrong id byte") ;
   for(i=vfbyte(); i>0; i--)
      (void)vfbyte() ;
   k = vfquad() ;
   check_checksum (k, curfnt->checksum, curfnt->name);
   k = (integer)(alpha * (real)vfquad()) ;
   if (k > curfnt->designsize + 2 || k < curfnt->designsize - 2) {
      (void)sprintf(errbuf,"Design size mismatch in font %s", name) ;
      error(errbuf) ;
   }
/*
 * Now we look for font definitions.
 */
   fm = NULL ;
   while ((cmd=vfbyte())>=243) {
      if (cmd>246)
         badvf("unexpected command in preamble") ;
      newf = vfontdef(scaledsize, cmd-242) ;
      if (fm)
         fm->next = newf ;
      else curfnt->localfonts = newf ;
      fm = newf ;
      fm->next = NULL ; /* FIFO */
   }
/*
 *   Now we get down to the serious business of reading character definitions.
 */
   do {
      if (cmd==242) {
         length = vfquad() + 2 ;
         if (length<2) badvf("negative length packet") ;
         if (length>65535) badvf("packet too long") ;
         cc = vfquad() ;
         if (cc<0 || cc>=no_of_chars) badvf("character code out of range") ;
         cd = curfnt->chardesc + cc ;
         cd->TFMwidth = scalewidth(vfquad(), scaledsize) ;
      } else {
         length = cmd + 2;
         cc = vfbyte() ;
         cd = curfnt->chardesc + cc ;
         cd->TFMwidth = scalewidth(vftrio(), scaledsize) ;
      }
      maxcc = (maxcc<cc) ? cc : maxcc;
      if (cd->TFMwidth >= 0)
         cd->pixelwidth = ((integer)(conv*cd->TFMwidth+0.5)) ;
      else
         cd->pixelwidth = -((integer)(conv*-cd->TFMwidth+0.5)) ;
      cd->flags = EXISTS ;
      cd->flags2 = EXISTS ;
      if (bytesleft < length) {
#ifdef DEBUG
          if (dd(D_MEM))
             (void)fprintf(stderr,
#ifdef SHORTINT
                   "Allocating new raster memory (%ld req, %ld left)\n",
#else
                   "Allocating new raster memory (%d req, %ld left)\n",
#endif
                                length, bytesleft) ;
#endif /* DEBUG */
          if (length > MINCHUNK) {
             tempr = (quarterword *)mymalloc((integer)length) ;
             bytesleft = 0 ;
          } else {
             raster = (quarterword *)mymalloc((integer)RASTERCHUNK) ;
             tempr = raster ;
             bytesleft = RASTERCHUNK - length ;
             raster += length ;
         }
      } else {
         tempr = raster ;
         bytesleft -= length ;
         raster += length ;
      }
      cd->packptr = tempr ;
      length -= 2 ;
      *tempr++ = length / 256 ;
      *tempr++ = length % 256 ;
         for (; length>0; length--)
            *tempr++ = vfbyte() ;
      cmd = vfbyte() ;
   } while (cmd < 243) ;
   if (cmd != 248)
      badvf("missing postamble") ;
   (void)fclose(vffile) ;
   curfnt->loaded = 2 ;
   if (maxcc+1<no_of_chars) {
      curfnt->chardesc = (chardesctype *)
         xrealloc(curfnt->chardesc,
                  (maxcc+1) * (integer)sizeof(chardesctype));
      curfnt->maxchars=maxcc+1;
   }
   return (1) ;
}
