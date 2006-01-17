/* dvipng.c */

/************************************************************************

  Part of the dvipng distribution

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
  02111-1307, USA.

  Copyright (C) 2002-2005 Jan-�ke Larsson

************************************************************************/

/**********************************************************************
 ****************************  Intro  *********************************
 **********************************************************************
 * This program translates TeX's DVI-Code into Portable Network Graphics.
 *
 * Recipe:
 * Take a simple dvi2?? converter, dvilj was found suitable.
 * Read and relish the code of xdvi and dvips
 * Choose a png-drawing library with a simple interface and support
 *  on many platforms: gd
 * Stir, sprinkle with some insights of your own, and enjoy
 *
 **********************************************************************
 */

#define MAIN
#include "dvipng.h"

#ifdef MIKTEX
#  define main __cdecl Main
#endif        /* MIKTEX */
/**********************************************************************/
/*******************************  main  *******************************/
/**********************************************************************/
int main(int argc, char ** argv)
{
  bool parsestdin;
    
#ifdef TIMING
# ifdef HAVE_GETTIMEOFDAY
  gettimeofday(&Tp, NULL);
  timer = Tp.tv_sec + ((float)(Tp.tv_usec))/ 1000000.0;
# else
#  ifdef HAVE_FTIME
  ftime(&timebuffer);
  timer = timebuffer.time + (float)(timebuffer.millitm) / 1000.0;
#  endif
# endif
#endif
  /* setbuf(stderr, NULL); */

#ifdef HAVE_LIBKPATHSEA
  /* Use extra paths as used by dvips */
  kpse_set_program_name(argv[0],"dvips");
  /* If dvipng is not installed in the texmf tree, and _only_
   * SELFAUTO...  is used in texmf.cnf, kpathsea will not find a)
   * Virtual fonts b) ps2pk.map or psfonts.map c) PostScript fonts
   *
   * We adjust these things here
   */
# ifdef ENV_SELFAUTOLOC
  putenv(ENV_SELFAUTOLOC);
# endif
# ifdef ENV_SELFAUTODIR
  putenv(ENV_SELFAUTODIR);
# endif
# ifdef ENV_SELFAUTOPARENT
  putenv(ENV_SELFAUTOPARENT);
# endif
  kpse_set_program_enabled (kpse_pk_format, makeTexPK, kpse_src_compile);
#endif

  initcolor();
  parsestdin = DecodeArgs(argc, argv);

#ifdef HAVE_LIBKPATHSEA
  if (user_mfmode)
    if (user_bdpi)
      kpse_init_prog("DVIPNG", user_bdpi, user_mfmode, "cmr10");
    else {
      Warning("--mfmode given without --bdpi");
      /* this will give a lot of warnings but... */
      kpse_init_prog("DVIPNG", 300, user_mfmode, "cmr10");
    }
  else
    kpse_init_prog("DVIPNG", 300, "cx", "cmr10");
#endif

#ifdef HAVE_FT2_OR_LIBT1
  InitPSFontMap();
#endif

  DrawPages();

  if (parsestdin) {
    char    line[STRSIZE];

    printf("%s> ",dvi!=NULL?dvi->name:"");
    fgets(line,STRSIZE,stdin);
    while(!feof(stdin)) {
      DecodeString(line);
      if (dvi!=NULL) {
	if (DVIReOpen(dvi))
	  flags &= ~PREVIEW_LATEX_TIGHTPAGE;
	DrawPages();
      }
      printf("%s> ",dvi!=NULL?dvi->name:"");
      fgets(line,STRSIZE,stdin);
    }
    printf("\n");
  }

#ifdef TIMING
# ifdef HAVE_GETTIMEOFDAY
  gettimeofday(&Tp, NULL);
  timer = (Tp.tv_sec + (float)(Tp.tv_usec)/1000000.0) - timer;
# else
#  ifdef HAVE_FTIME
  ftime(&timebuffer);
  timer = (timebuffer.time + (float)(timebuffer.millitm)/1000.0) - timer;
#  endif
# endif
  
  if (ndone > 0) 
    fprintf(stderr,
	    "Time of complete run: %.2f s, %d page(s), %.4f s/page.\n",
	    timer, ndone, timer / ndone);
  if (my_toc >= 0.0001) 
    fprintf(stderr,
	    "Thereof in TIC/TOC region %.5f s.\n",my_toc);
#endif

  ClearFonts();
  DVIClose(dvi);
  ClearColorNames();
#ifdef HAVE_FT2_OR_LIBT1
  ClearPSFontMap();
  ClearEncoding();
#endif
#ifdef HAVE_FT2
  if (libfreetype!=NULL && FT_Done_FreeType(libfreetype)) 
    Fatal("an error occured during freetype destruction"); 
  libfreetype = NULL;
#endif  
#ifdef HAVE_LIBT1
  if (libt1!=NULL && T1_CloseLib())
    Fatal("an error occured during t1lib destruction"); 
  libt1 = NULL;
#endif  

  exit(exitcode);
}



