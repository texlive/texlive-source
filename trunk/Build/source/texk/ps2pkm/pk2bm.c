/*
 * NAME
 *      pk2bm - create a bitmap from a TeX pkfont
 * SYNOPSIS:
 *	pk2bm [-bh] [-W width] [-H height] {-c char|-o octchar} pkfont 
 * DESCRIPTION
 *	This program generates a bitmap (ASCII file) which can be used
 *	to create X11 applications. The bitmap file consists of all
 *	pixels of the specified character (via the -c or -o option) from 
 *	the given pkfont. The format is described in bitmap(X11).
 *
 *	The pkfont is a packed fontfile generated by gftopk(TeX) from a 
 *	gffont. A gffont is the output of METAFONT a program to design fonts
 *	in a device independant way.
 *
 *	With the -b flag a bitmap is generated in which all black pixels are
 *	drawn as a `*' and all white bits as a `.'. With the -h flag a 
 *	hexadecimal bitmap dump is generated. 
 *
 *      The -W and/or -H options can be used to create a bitmap of
 *	respectivally `width' and `height' pixels. The pk-bitmap will in
 *	this case be centered according to these new dimensions.
 *
 *	The output is written to the standard output.
 * SEE ALSO
 *	``METAFONT'', Donald Knuth.
 *	``The PKtype processor'', belonging to the METAFONTware.
 *	bitmap(X11), gftopk(TeX), pktype(TeX), ps2pk(1)
 * AUTHOR
 *	Piet Tutelaers
 *	rcpt@urc.tue.nl
 */
#include <stdio.h>
#include "pkin.h"

int main(int argc, char *argv[])
{
   int done, C = 0, c, h = 0, w = 0, row, col;
   unsigned int bitmap = 0, hexmap = 0;
   halfword *word;
   quarterword lsbf();
   void dots();
   chardesc cd;
   char *myname = "pk2bm", *pkname;
   int atoo(char *);
	
   while (--argc > 0 && (*++argv)[0] == '-') {
      done=0;
      while ((!done) && (c = *++argv[0]))	/* allow -bcK as an option */
         switch (c) {
      	 case 'c':
      	    if (*++argv[0] == '\0') {
      	       argc--;  argv++;
      	    }
      	    C = *argv[0];
      	    done = 1; break;
      	 case 'o':
      	    if (*++argv[0] == '\0') {
      	       argc--;  ++argv;
      	    }
      	    C = atoo(argv[0]);
      	    done = 1; break;
      	 case 'H':
      	    if (*++argv[0] == '\0') {
      	       argc--; argv++;
      	    }
      	    h = atoi(argv[0]);
            done=1;
      	    break;
      	 case 'W':
      	    if (*++argv[0] == '\0') {
      	       argc--; argv++;
      	    }
      	    w = atoi(argv[0]);
	    done=1;
      	    break;
      	 case 'b':
      	    bitmap = 1;
      	    break;
      	 case 'h':
      	    hexmap = 1;
      	    break;
      	 default:
      	    printf("%s: %c illegal option\n", myname, c);
      	    exit(1);
      	 }
   }

   if (argc == 0) {
      printf("Usage: %s [-bh] {-c char|-o octchar} [-W width -H height] pkfile\n", myname);
      exit(1);
   }

#ifdef WIN32
   setmode(fileno(stdout), _O_BINARY);
#endif

   pkname = argv[0];

   if (readchar(pkname, C, &cd)) {
      int H, dh, W, dw, bitsleft;
      halfword nextword; quarterword nextbyte;

      H=cd.cheight; if (h==0) h=H; dh=(h-H)/2;
      W=cd.cwidth; if (w==0) w=W; dw=(w-W)/2;
	   
      if (bitmap || hexmap) {
         printf("character : %d (%c)\n", (int) cd.charcode, 
	         (char)cd.charcode);
         printf("   height : %d\n", (int) cd.cheight);
         printf("    width : %d\n", (int) cd.cwidth);
         printf("     xoff : %d\n", (int) cd.xoff);
         printf("     yoff : %d\n", (int) cd.yoff);
      }
      else {
         printf("#define %s_%c_width \t %d\n",
              pkname, (char)cd.charcode, (int) w);
         printf("#define %s_%c_height \t %d\n",
              pkname, (char)cd.charcode, (int) h);
         printf("#define %s_%c_xoff \t %d\n",
              pkname, (char)cd.charcode, (int) dw);
         printf("#define %s_%c_yoff \t %d\n",
              pkname, (char)cd.charcode, (int) dh);
         printf("static char %s_%c_bits[] = {", 
   	      pkname, (char)cd.charcode);
      }
      for (row=0; row<h-H-dh; row++) {
          printf("\n  ");
          for (col=0; col<w; col+=8)
             if (bitmap) dots(0, w-col>7? 8: w-col);
             else if (hexmap) printf("%02x", 0x00);
             else printf(" 0x%02x, ", 0x00);
      }
      word = cd.raster;
      for (row=h-H-dh; row<h-dh; row++) {
         bitsleft = dw; nextword = 0;
         printf("\n  ");
         for (col=0; col<w; col+=8) {
            if (bitsleft>=8) {
               nextbyte = nextword >> (bitsleft-8) & 0xff;
	       bitsleft-=8;
	       
               if (bitmap) dots(nextbyte, w-col>7? 8: w-col); 
               else if (hexmap) printf("%02x", nextbyte);
               else printf(" 0x%02x, ", lsbf(nextbyte));
	    }
	    else {
	       nextbyte = nextword << (8-bitsleft) & 0xff;
	       if (col-dw<W)
	          nextword=(*word++);
	       else nextword=0;
	       nextbyte = nextbyte | (nextword >> (16-(8-bitsleft))
		                           & 0xff);
	       bitsleft = 16 - (8-bitsleft);
	       
               if (bitmap) dots(nextbyte, w-col>7? 8: w-col);
               else if (hexmap) printf("%02x", nextbyte);
               else printf(" 0x%02x, ", lsbf(nextbyte));
	    }
	 }
      }
      for (row=h-dh; row<h; row++) {
          printf("\n  ");
	  for (col=0; col<w; col+=8)
             if (bitmap) dots(0, w-col>7? 8: w-col);
             else if (hexmap) printf("%02x", 0x00);
             else printf(" 0x%02x, ", 0x00);
      }
      if ((!bitmap) && (!hexmap)) printf("};\n");
      else putchar('\n');
   }
   return 0;
}

/*
 * lsbf() transforms a byte (least significant bit first).
 */
quarterword lsbf(u)
quarterword u;
{
	int i; quarterword bit, result = 0;

	for (i=0; i<8; i++) {
	   bit = u & 001;
	   result = (result<<1) | bit;
	   u = u >> 1;
	}
	return result;
}

/*
 * dots() print byte: `*' for a black and `.' for a white pixel
 */
void dots(u, n)
quarterword u; int n;
{  unsigned int bit;

   bit=1<<7;
   for ( ;n>0 ;n--) {
      if (u & bit) putchar('*');
      else putchar('.');
      bit>>=1;
   }
}

int atoo(char *oct)
{  int octal = 0;
   while (*oct != '\0') octal = 8*octal + (*oct++) - '0';
   return octal & 0xff;
}
