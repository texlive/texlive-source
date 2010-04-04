/* Copyright (C) 2000-2008 by George Williams */
/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.

 * The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "pfaedit.h"
#include <stdio.h>
#include <math.h>
#include "splinefont.h"
#include "views.h"
#include "stemdb.h"
#include <utype.h>
#include <chardata.h>
#include "edgelist.h"

/* to create a type 1 font we must come up with the following entries for the
  private dictionary:
    BlueValues	-- an array of 2n entries where Blue[2i]<Blue[2i+1] max n=7, Blue[i]>0
    OtherBlues	-- (optional) OtherBlue[i]<0
    	(blue zones should be at least 3 units appart)
    StdHW	-- (o) array with one entry, standard hstem height
    StdVW	-- (o) ditto vstem width
    StemSnapH	-- (o) up to 12 numbers containing various hstem heights (includes StdHW), small widths first
    StemSnapV	-- (o) ditto, vstem
This file has routines to figure out at least some of these

Also other routines to guess at good per-character hints
*/

static void AddBlue(real val, real array[5], int force) {
    val = rint(val);
    if ( !force && (val<array[0]-array[1] || val >array[0]+array[1] ))
return;		/* Outside of one sd */
    if ( array[3]==0 && array[4]==0 )
	array[3] = array[4] = val;
    else if ( val>array[4] )
	array[4] = val;
    else if ( val<array[3] )
	array[3] = val;
}

static void MergeZones(real zone1[5], real zone2[5]) {
    if ( zone1[2]!=0 && zone2[2]!=0 &&
	    ((zone1[4]+3>zone2[3] && zone1[3]<=zone2[3]) ||
	     (zone2[4]+3>zone1[3] && zone2[3]<=zone1[3]) )) {
	if (( zone2[0]<zone1[0]-zone1[1] || zone2[0] >zone1[0]+zone1[1] ) &&
		( zone1[0]<zone2[0]-zone2[1] || zone1[0] >zone2[0]+zone2[1] ))
	    /* the means of the zones are too far appart, don't merge em */;
	else {
	    if ( zone1[0]<zone2[0] ) zone2[0] = zone1[0];
	    if ( zone1[1]>zone2[1] ) zone2[1] = zone1[1];
	}
	zone1[2] = 0;
    }
}

/* I can deal with latin, greek and cyrillic because the they all come from */
/*  the same set of letter shapes and have all evolved together and have */
/*  various common features (ascenders, descenders, lower case, etc.). Other */
/*  scripts don't fit */
void FindBlues( SplineFont *sf, int layer, real blues[14], real otherblues[10]) {
    real caph[5], xh[5], ascenth[5], digith[5], descenth[5], base[5];
    real otherdigits[5];
    int i, j, k;
    DBounds b;

    /* Go through once to get some idea of the average value so we can weed */
    /*  out undesireables */
    caph[0] = caph[1] = caph[2] = xh[0] = xh[1] = xh[2] = 0;
    ascenth[0] = ascenth[1] = ascenth[2] = digith[0] = digith[1] = digith[2] = 0;
    descenth[0] = descenth[1] = descenth[2] = base[0] = base[1] = base[2] = 0;
    otherdigits[0] = otherdigits[1] = otherdigits[2] = 0;
    for ( i=0; i<sf->glyphcnt; ++i ) {
	if ( sf->glyphs[i]!=NULL && sf->glyphs[i]->layers[layer].splines!=NULL ) {
	    int enc = sf->glyphs[i]->unicodeenc;
#ifndef LUA_FF_LIB
	    const unichar_t *upt;
#endif
	    if ( enc<0x10000 && isalnum(enc) &&
		    ((enc>=32 && enc<128 ) || enc == 0xfe || enc==0xf0 || enc==0xdf ||
		      enc==0x131 ||
		     (enc>=0x391 && enc<=0x3f3 ) ||
		     (enc>=0x400 && enc<=0x4e9 ) )) {
		/* no accented characters (or ligatures) */
#ifndef LUA_FF_LIB
		if ( unicode_alternates[enc>>8]!=NULL &&
			(upt =unicode_alternates[enc>>8][enc&0xff])!=NULL &&
			upt[1]!='\0' )
    continue;
#endif
		SplineCharFindBounds(sf->glyphs[i],&b);
		if ( b.miny==0 && b.maxy==0 )
    continue;
		if ( enc=='g' || enc=='j' || enc=='p' || enc=='q' || enc=='y' ||
			enc==0xfe || 
			enc==0x3c1 /* rho */ ||
			enc==0x3c6 /* phi */ || 
			enc==0x3c7 /* chi */ || 
			enc==0x3c8 /* psi */ ||
			enc==0x440 /* cyr er */ || 
			enc==0x443 /* cyr u */ || 
			enc==0x444 /* cyr ef */) {
		    descenth[0] += b.miny;
		    descenth[1] += b.miny*b.miny;
		    ++descenth[2];
		} else if ( enc=='x' || enc=='r' || enc=='o' || enc=='e' || 
		               enc=='s' || enc=='c' || enc=='h' || enc=='k' || 
			       enc=='l' || enc=='m' || enc=='n' || 
			       enc==0x3b5 /* epsilon */ || 
			       enc==0x3b9 /* iota */ ||
			       enc==0x3ba /* kappa */ ||
			       enc==0x3bf /* omicron */ ||
			       enc==0x3c3 /* sigma */ ||
			       enc==0x3c5 /* upsilon */ ||
			       enc==0x430 /* cyr a */ ||
			       enc==0x432 /* cyr ve */ ||
			       enc==0x433 /* cyr ge */ ||
			       enc==0x435 /* cyr e */ ||
			       enc==0x436 /* cyr zhe */ ||
			       enc==0x438 /* cyr i */ ||
			       enc==0x43a /* cyr ka */ ||
			       enc==0x43d /* cyr en */ ||
			       enc==0x43e /* cyr o */ ||
			       enc==0x441 /* cyr es */ ||
			       enc==0x445 /* cyr ha */ ||
			       enc==0x447 /* cyr che */ ||
			       enc==0x448 /* cyr sha */ ||
			       enc==0x44f /* cyr ya */ ){
		    base[0] += b.miny;
		    base[1] += b.miny*b.miny;
		    ++base[2];
		}
		/* careful of lowercase digits, 6 and 8 should be ascenders */
		if ( enc=='6' || enc=='8' ) {
		    digith[0] += b.maxy;
		    digith[1] += b.maxy*b.maxy;
		    ++digith[2];
		} else if ( enc<0x10000 && isdigit(enc) ) {
		    otherdigits[0] += b.maxy;
		    otherdigits[1] += b.maxy*b.maxy;
		    ++otherdigits[2];
		} else if ( enc<0x10000 && isupper(enc) && enc!=0x462 && enc!=0x490 ) {
		    caph[0] += b.maxy;
		    caph[1] += b.maxy*b.maxy;
		    ++caph[2];
		} else if ( enc=='b' || enc=='d' || enc=='f' || enc=='h' || enc=='k' ||
			enc == 'l' || enc==0xf0 || enc==0xfe || enc == 0xdf ||
			enc == 0x3b2 || enc==0x3b6 || enc==0x3b8 || enc==0x3bb ||
			enc == 0x3be ||
			enc == 0x431 /* cyr be */ /* || enc == 0x444 - ef may have varible height */) {
		    ascenth[0] += b.maxy;
		    ascenth[1] += b.maxy*b.maxy;
		    ++ascenth[2];
		} else if ( enc=='c' || enc=='e' || enc=='o' || enc=='s' || enc=='u' || 
		            enc=='u' || enc=='v' || enc=='w' || enc=='x' || enc=='y' || 
			    enc=='z' || 
			    enc==0x3b5 /* epsilon */ ||
			    enc==0x3b9 /* iota */ ||
			    enc==0x3ba /* kappa */ ||
			    enc==0x3bc /* mu */ ||
			    enc==0x3bd /* nu */ ||
			    enc==0x3bf /* omicron */ ||
			    enc==0x3c0 /* pi */ ||
			    enc==0x3c1 /* rho */ ||
			    enc==0x3c5 /* upsilon */ ||
			    enc==0x433 /* cyr ge */ ||
			    enc==0x435 /* cyr e */ ||
			    enc==0x436 /* cyr zhe */ ||
			    enc==0x438 /* cyr i */ ||
			    enc==0x43b /* cyr el */ ||
			    enc==0x43d /* cyr en */ ||
			    enc==0x43e /* cyr o */ ||
			    enc==0x43f /* cyr pe */ ||
			    enc==0x440 /* cyr er */ ||
			    enc==0x441 /* cyr es */ ||
			    enc==0x442 /* cyr te */ ||
			    enc==0x443 /* cyr u */ ||
			    enc==0x445 /* cyr ha */ ||
			    enc==0x446 /* cyr tse */ ||
			    enc==0x447 /* cyr che */ ||
			    enc==0x448 /* cyr sha */ ||
			    enc==0x449 /* cyr shcha */ ||
			    enc==0x44a /* cyr hard sign */ ||
			    enc==0x44b /* cyr yery */ ||
			    enc==0x44c /* cyr soft sign */ ||
			    enc==0x44d /* cyr reversed e */ ||
			    enc==0x44f /* cyr ya */ ) {
		    xh[0] += b.maxy;
		    xh[1] += b.maxy*b.maxy;
		    ++xh[2];
		}
	    }
	}
	if ( !ff_progress_next())
    break;
    }
    if ( otherdigits[2]>0 && digith[2]>0 ) {
	if ( otherdigits[0]/otherdigits[2] >= .95*digith[0]/digith[2] ) {
	    /* all digits are about the same height, not lowercase */
	    digith[0] += otherdigits[0];
	    digith[1] += otherdigits[1];
	    digith[2] += otherdigits[2];
	}
    }

    if ( xh[2]>1 ) {
	xh[1] = sqrt((xh[1]-xh[0]*xh[0]/xh[2])/(xh[2]-1));
	xh[0] /= xh[2];
    }
    if ( ascenth[2]>1 ) {
	ascenth[1] = sqrt((ascenth[1]-ascenth[0]*ascenth[0]/ascenth[2])/(ascenth[2]-1));
	ascenth[0] /= ascenth[2];
    }
    if ( caph[2]>1 ) {
	caph[1] = sqrt((caph[1]-caph[0]*caph[0]/caph[2])/(caph[2]-1));
	caph[0] /= caph[2];
    }
    if ( digith[2]>1 ) {
	digith[1] = sqrt((digith[1]-digith[0]*digith[0]/digith[2])/(digith[2]-1));
	digith[0] /= digith[2];
    }
    if ( base[2]>1 ) {
	base[1] = sqrt((base[1]-base[0]*base[0]/base[2])/(base[2]-1));
	base[0] /= base[2];
    }
    if ( descenth[2]>1 ) {
	descenth[1] = sqrt((descenth[1]-descenth[0]*descenth[0]/descenth[2])/(descenth[2]-1));
	descenth[0] /= descenth[2];
    }

    /* we'll accept values between +/- 1sd of the mean */
    /* array[0] == mean, array[1] == sd, array[2] == cnt, array[3]=min, array[4]==max */
    if ( base[0]+base[1]<0 ) base[1] = -base[0];	/* Make sure 0 is within the base bluezone */
    caph[3] = caph[4] = 0;
    xh[3] = xh[4] = 0;
    ascenth[3] = ascenth[4] = 0;
    digith[3] = digith[4] = 0;
    descenth[3] = descenth[4] = 0;
    base[3] = base[4] = 0;
    for ( i=0; i<sf->glyphcnt; ++i ) if ( sf->glyphs[i]!=NULL ) {
	int enc = sf->glyphs[i]->unicodeenc;
#ifndef LUA_FF_LIB
	const unichar_t *upt;
#endif
	if ( enc<0x10000 && isalnum(enc) &&
		((enc>=32 && enc<128 ) || enc == 0xfe || enc==0xf0 || enc==0xdf ||
		 (enc>=0x391 && enc<=0x3f3 ) ||
		 (enc>=0x400 && enc<=0x4e9 ) )) {
	    /* no accented characters (or ligatures) */
#ifndef LUA_FF_LIB
	    if ( unicode_alternates[enc>>8]!=NULL &&
		    (upt =unicode_alternates[enc>>8][enc&0xff])!=NULL &&
		    upt[1]!='\0' )
    continue;
#endif
	    SplineCharFindBounds(sf->glyphs[i],&b);
	    if ( b.miny==0 && b.maxy==0 )
    continue;
	    if ( enc=='g' || enc=='j' || enc=='p' || enc=='q' || enc=='y' ||
		    enc==0xfe || enc == 0x3c6 || enc==0x3c8 ||
		    enc==0x440 || enc==0x443 || enc==0x444) {
		AddBlue(b.miny,descenth,false);
	    } else {
		/* O and o get forced into the baseline blue value even if they*/
		/*  are beyond 1 sd */
		AddBlue(b.miny,base,enc=='O' || enc=='o');
	    }
	    if ( enc<0x10000 && isdigit(enc)) {
		AddBlue(b.maxy,digith,false);
	    } else if ( enc<0x10000 && isupper(enc)) {
		AddBlue(b.maxy,caph,enc=='O');
	    } else if ( enc=='b' || enc=='d' || enc=='f' || enc=='h' || enc=='k' ||
		    enc == 'l' || enc=='t' || enc==0xf0 || enc==0xfe || enc == 0xdf ||
		    enc == 0x3b2 || enc==0x3b6 || enc==0x3b8 || enc==0x3bb ||
		    enc == 0x3be ||
		    enc == 0x431 ) {
		AddBlue(b.maxy,ascenth,false);
	    } else if ( enc=='c' || enc=='e' || enc=='o' || enc=='s' || enc=='u' || 
			enc=='u' || enc=='v' || enc=='w' || enc=='x' || enc=='y' || 
			enc=='z' || 
			enc==0x3b5 /* epsilon */ ||
			enc==0x3b9 /* iota */ ||
			enc==0x3ba /* kappa */ ||
			enc==0x3bc /* mu */ ||
			enc==0x3bd /* nu */ ||
			enc==0x3bf /* omicron */ ||
			enc==0x3c0 /* pi */ ||
			enc==0x3c1 /* rho */ ||
			enc==0x3c5 /* upsilon */ ||
			enc==0x433 /* cyr ge */ ||
			enc==0x435 /* cyr e */ ||
			enc==0x436 /* cyr zhe */ ||
			enc==0x438 /* cyr i */ ||
			enc==0x43b /* cyr el */ ||
			enc==0x43d /* cyr en */ ||
			enc==0x43e /* cyr o */ ||
			enc==0x43f /* cyr pe */ ||
			enc==0x440 /* cyr er */ ||
			enc==0x441 /* cyr es */ ||
			enc==0x442 /* cyr te */ ||
			enc==0x443 /* cyr u */ ||
			enc==0x445 /* cyr ha */ ||
			enc==0x446 /* cyr tse */ ||
			enc==0x447 /* cyr che */ ||
			enc==0x448 /* cyr sha */ ||
			enc==0x449 /* cyr shcha */ ||
			enc==0x44a /* cyr hard sign */ ||
			enc==0x44b /* cyr yery */ ||
			enc==0x44c /* cyr soft sign */ ||
			enc==0x44d /* cyr reversed e */ ||
			enc==0x44f /* cyr ya */ ) {
		AddBlue(b.maxy,xh,enc=='o' || enc=='x');
	    }
	}
    }

    /* the descent blue zone merges into the base zone */
    MergeZones(descenth,base);
    MergeZones(xh,base);
    MergeZones(ascenth,caph);
    MergeZones(digith,caph);
    MergeZones(xh,caph);
    MergeZones(ascenth,digith);
    MergeZones(xh,digith);

    if ( otherblues!=NULL )
	for ( i=0; i<10; ++i )
	    otherblues[i] = 0;
    for ( i=0; i<14; ++i )
	blues[i] = 0;

    if ( otherblues!=NULL && descenth[2]!=0 ) {
	otherblues[0] = descenth[3];
	otherblues[1] = descenth[4];
    }
    i = 0;
    if ( base[2]==0 && (xh[2]!=0 || ascenth[2]!=0 || caph[2]!=0 || digith[2]!=0 )) {
	/* base line blue value must be present if any other value is */
	/*  make one up if we don't have one */
	blues[0] = -20;
	blues[1] = 0;
	i = 2;
    } else if ( base[2]!=0 ) {
	blues[0] = base[3];
	blues[1] = base[4];
	i = 2;
    }
    if ( xh[2]!=0 ) {
	blues[i++] = xh[3];
	blues[i++] = xh[4];
    }
    if ( caph[2]!=0 ) {
	blues[i++] = caph[3];
	blues[i++] = caph[4];
    }
    if ( digith[2]!=0 ) {
	blues[i++] = digith[3];
	blues[i++] = digith[4];
    }
    if ( ascenth[2]!=0 ) {
	blues[i++] = ascenth[3];
	blues[i++] = ascenth[4];
    }

    for ( j=0; j<i; j+=2 ) for ( k=j+2; k<i; k+=2 ) {
	if ( blues[j]>blues[k] ) {
	    real temp = blues[j];
	    blues[j]=blues[k];
	    blues[k] = temp;
	    temp = blues[j+1];
	    blues[j+1] = blues[k+1];
	    blues[k+1] = temp;
	}
    }
}

void ElFreeEI(EIList *el) {
    EI *e, *next;

    for ( e = el->edges; e!=NULL; e = next ) {
	next = e->next;
	free(e);
    }
}

static int EIAddEdge(Spline *spline, real tmin, real tmax, EIList *el) {
    EI *new = gcalloc(1,sizeof(EI));
    real min, max, temp;
    Spline1D *s;
    real dxdtmin, dxdtmax, dydtmin, dydtmax;

    new->spline = spline;
    new->tmin = tmin;
    new->tmax = tmax;

    s = &spline->splines[1];
    if (( dydtmin = (3*s->a*tmin + 2*s->b)*tmin + s->c )<0 ) dydtmin = -dydtmin;
    if (( dydtmax = (3*s->a*tmax + 2*s->b)*tmax + s->c )<0 ) dydtmax = -dydtmax;
    s = &spline->splines[0];
    if (( dxdtmin = (3*s->a*tmin + 2*s->b)*tmin + s->c )<0 ) dxdtmin = -dxdtmin;
    if (( dxdtmax = (3*s->a*tmax + 2*s->b)*tmax + s->c )<0 ) dxdtmax = -dxdtmax;
    
    /*s = &spline->splines[0];*/
    min = ((s->a * tmin + s->b)* tmin + s->c)* tmin + s->d;
    max = ((s->a * tmax + s->b)* tmax + s->c)* tmax + s->d;
    if ( tmax==1 ) max = spline->to->me.x;	/* beware rounding errors */
    if ( !el->leavetiny && floor(min)==floor(max) ) {	/* If it doesn't cross a pixel boundary then it might as well be vertical */
	if ( tmin==0 ) max = min;
	else if ( tmax==1 ) min = max;
	else max = min;
    }
    if ( min==max )
	new->vert = true;
    else if ( min<max )
	new->hup = true;
    else {
	temp = min; min = max; max=temp;
    }
    if ( !el->leavetiny && min+1>max ) new->almostvert = true;
    if ( 40*dxdtmin<dydtmin ) new->vertattmin = true;
    if ( 40*dxdtmax<dydtmax ) new->vertattmax = true;
    /*if ( new->vertattmin && new->vertattmax && s->a==0 && s->b==0 ) new->almostvert = true;*/
    new->coordmin[0] = min; new->coordmax[0] = max;
    if ( el->coordmin[0]>min )
	el->coordmin[0] = min;
    if ( el->coordmax[0]<max )
	el->coordmax[0] = max;

    s = &spline->splines[1];
    min = ((s->a * tmin + s->b)* tmin + s->c)* tmin + s->d;
    max = ((s->a * tmax + s->b)* tmax + s->c)* tmax + s->d;
    if ( tmax==1 ) max = spline->to->me.y;
    if ( !el->leavetiny && floor(min)==floor(max) ) {	/* If it doesn't cross a pixel boundary then it might as well be horizontal */
	if ( tmin==0 ) max = min;
	else if ( tmax==1 ) min = max;
	else max = min;
    }
    if ( min==max )
	new->hor = true;
    else if ( min<max )
	new->vup = true;
    else {
	temp = min; min = max; max=temp;
    }
    if ( !el->leavetiny && min+1>max ) new->almosthor = true;
    if ( 40*dydtmin<dxdtmin ) new->horattmin = true;
    if ( 40*dydtmax<dxdtmax ) new->horattmax = true;
    /*if ( new->horattmin && new->horattmax && s->a==0 && s->b==0 ) new->almosthor = true;*/
    new->coordmin[1] = min; new->coordmax[1] = max;
    if ( el->coordmin[1]>min )
	el->coordmin[1] = min;
    if ( el->coordmax[1]<max )
	el->coordmax[1] = max;

    if ( new->hor && new->vert ) {
	/* This spline is too small for us to notice */
	free(new);
return( false );
    } else {
	new->next = el->edges;
	el->edges = new;

	if ( el->splinelast!=NULL )
	    el->splinelast->splinenext = new;
	el->splinelast = new;
	if ( el->splinefirst==NULL )
	    el->splinefirst = new;

return( true );
    }
}

static void EIAddSpline(Spline *spline, EIList *el) {
    extended ts[6], temp;
    int i, j, base, last;

    ts[0] = 0; ts[5] = 1.0;
    SplineFindExtrema(&spline->splines[0],&ts[1],&ts[2]);
    SplineFindExtrema(&spline->splines[1],&ts[3],&ts[4]);
    /* avoid teeny tiny segments, they just confuse us */
    SplineRemoveExtremaTooClose(&spline->splines[0],&ts[1],&ts[2]);
    SplineRemoveExtremaTooClose(&spline->splines[1],&ts[3],&ts[4]);
    for ( i=0; i<4; ++i ) for ( j=i+1; j<5; ++j ) {
	if ( ts[i]>ts[j] ) {
	    temp = ts[i];
	    ts[i] = ts[j];
	    ts[j] = temp;
	}
    }
    for ( base=0; ts[base]==-1; ++base);
    for ( i=5; i>base ; --i ) {
	if ( ts[i]==ts[i-1] ) {
	    for ( j=i-1; j>base; --j )
		ts[j] = ts[j-1];
	    ts[j] = -1;
	    ++base;
	}
    }
    last = base;
    for ( i=base; i<5 ; ++i )
	if ( EIAddEdge(spline,ts[last],ts[i+1],el) )
	    last = i+1;
}

void ELFindEdges(SplineChar *sc, EIList *el) {
    Spline *spline, *first;
    SplineSet *spl;

    el->sc = sc;
    if ( sc->layers[el->layer].splines==NULL )
return;

    el->coordmin[0] = el->coordmax[0] = sc->layers[el->layer].splines->first->me.x;
    el->coordmin[1] = el->coordmax[1] = sc->layers[el->layer].splines->first->me.y;

    for ( spl = sc->layers[el->layer].splines; spl!=NULL; spl = spl->next ) if ( spl->first->prev!=NULL && spl->first->prev->from!=spl->first ) {
	first = NULL;
	for ( spline = spl->first->next; spline!=NULL && spline!=first; spline=spline->to->next ) {
	    EIAddSpline(spline,el);
	    if ( first==NULL ) first = spline;
	}
	if ( el->splinefirst!=NULL && spl->first->prev!=NULL )
	    el->splinelast->splinenext = el->splinefirst;
	el->splinelast = NULL; el->splinefirst = NULL;
    }
}

static int IsBiggerSlope(EI *test, EI *base, int major) {
    int other = !major;
    real tdo, tdm, bdo, bdm, t;

    if (( major && test->vup ) || (!major && test->hup))
	t = test->tmin;
    else
	t = test->tmax;
    tdm = (3*test->spline->splines[major].a*t + 2*test->spline->splines[major].b)*t + test->spline->splines[major].c;
    tdo = (3*test->spline->splines[other].a*t + 2*test->spline->splines[other].b)*t + test->spline->splines[other].c;

    if (( major && base->vup ) || (!major && base->hup))
	t = base->tmin;
    else
	t = base->tmax;
    bdm = (3*base->spline->splines[major].a*t + 2*base->spline->splines[major].b)*t + base->spline->splines[major].c;
    bdo = (3*base->spline->splines[other].a*t + 2*base->spline->splines[other].b)*t + base->spline->splines[other].c;

    if ( tdm==0 && bdm==0 )
return( tdo > bdo );
    if ( tdo==0 )
return( tdo>0 );
    else if ( bdo==0 )
return( bdo>0 );

return( tdo/tdm > bdo/bdm );
}

void ELOrder(EIList *el, int major ) {
    int other = !major;
    int pos;
    EI *ei, *prev, *test;

    el->low = floor(el->coordmin[major]); el->high = ceil(el->coordmax[major]);
    el->cnt = el->high-el->low+1;
    el->ordered = gcalloc(el->cnt,sizeof(EI *));
    el->ends = gcalloc(el->cnt,1);

    for ( ei = el->edges; ei!=NULL ; ei=ei->next ) {
	pos = ceil(ei->coordmax[major])-el->low;
	el->ends[pos] = true;
	pos = floor(ei->coordmin[major])-el->low;
	ei->ocur = (ei->hup == ei->vup)?ei->coordmin[other]:ei->coordmax[other];
	ei->tcur = ((major && ei->vup) || (!major && ei->hup)) ?
			ei->tmin: ei->tmax;
	if ( major ) {
	    ei->up = ei->vup;
	    ei->hv = (ei->vert || ei->almostvert);
	    ei->hvbottom = ei->vup ? ei->vertattmin : ei->vertattmax;
	    ei->hvtop    =!ei->vup ? ei->vertattmin : ei->vertattmax;
	    if ( ei->hor || ei->almosthor)
    continue;
	} else {
	    ei->up = ei->hup;
	    ei->hv = (ei->hor || ei->almosthor);
	    ei->hvbottom = ei->hup ? ei->horattmin : ei->horattmax;
	    ei->hvtop    =!ei->hup ? ei->horattmin : ei->horattmax;
	    if ( ei->vert || ei->almostvert)
    continue;
	}
	if ( el->ordered[pos]==NULL || ei->ocur<el->ordered[pos]->ocur ) {
	    ei->ordered = el->ordered[pos];
	    el->ordered[pos] = ei;
	} else {
	    for ( prev=el->ordered[pos], test = prev->ordered; test!=NULL;
		    prev = test, test = test->ordered ) {
		if ( test->ocur>ei->ocur ||
			(test->ocur==ei->ocur && IsBiggerSlope(test,ei,major)))
	    break;
	    }
	    ei->ordered = test;
	    prev->ordered = ei;
	}
    }
}

static HintInstance *HIMerge(HintInstance *into, HintInstance *hi) {
    HintInstance *n, *first = NULL, *last = NULL;

    while ( into!=NULL && hi!=NULL ) {
	if ( into->begin<hi->begin ) {
	    n = into;
	    into = into->next;
	} else {
	    n = hi;
	    hi = hi->next;
	}
	if ( first==NULL )
	    first = n;
	else
	    last->next = n;
	last = n;
    }
    if ( into!=NULL ) {
	if ( first==NULL )
	    first = into;
	else
	    last->next = into;
    } else if ( hi!=NULL ) {
	if ( first==NULL )
	    first = hi;
	else
	    last->next = hi;
    }
return( first );
}

StemInfo *HintCleanup(StemInfo *stem,int dosort,int instance_count) {
    StemInfo *s, *p=NULL, *t, *pt, *sn;
    int swap;

    for ( s=stem; s!=NULL; p=s, s=s->next ) {
	if ( s->width<0 ) {
	    s->start += s->width;
	    s->width = -s->width;
	    s->ghost = true;
	}
	s->reordered = false;
	if ( p!=NULL && p->start> s->start )
	    dosort = true;
    }
    if ( dosort ) {
	for ( p=NULL, s=stem; s!=NULL; p=s, s = sn ) {
	    sn = s->next;
	    for ( pt=s, t=sn; t!=NULL; pt=t, t=t->next ) {
		if ( instance_count>1 &&
			t->u.unblended!=NULL && s->u.unblended!=NULL ) {
		    int temp = UnblendedCompare((*t->u.unblended)[0],(*s->u.unblended)[0],instance_count);
		    if ( temp==0 )
			swap = UnblendedCompare((*t->u.unblended)[1],(*s->u.unblended)[1],instance_count);
		    else
			swap = temp<0;
		} else if ( t->start<s->start )
		    swap=true;
		else if ( t->start>s->start )
		    swap = false;
		else
		    swap = (t->width<s->width);
		if ( swap ) {
		    s->next = t->next;
		    if ( pt==s ) {
			t->next = s;
			sn = s;
		    } else {
			t->next = sn;
			pt->next = s;
		    }
		    if ( p==NULL )
			stem = t;
		    else
			p->next = t;
		    pt = s;	/* swap s and t */
		    s = t;
		    t = pt;
		}
	    }
	}
	/* Remove duplicates */
	if ( stem!=NULL ) for ( p=stem, s=stem->next; s!=NULL; s = sn ) {
	    sn = s->next;
	    if ( p->start==s->start && p->width==s->width && p->hintnumber==s->hintnumber ) {
		p->where = HIMerge(p->where,s->where);
		s->where = NULL;
		p->next = sn;
		StemInfoFree(s);
	    } else
		p = s;
	}
    }
return( stem );
}

real EITOfNextMajor(EI *e, EIList *el, real sought_m ) {
    /* We want to find t so that Mspline(t) = sought_m */
    /*  the curve is monotonic */
    Spline1D *msp = &e->spline->splines[el->major];
    real new_t;
    real found_m;
    real t_mmax, t_mmin;

    if ( msp->a==0 && msp->b==0 ) {
	if ( msp->c == 0 ) {
	    IError("Hor/Vert line when not expected");
return( 0 );
	}
	new_t = (sought_m-msp->d)/(msp->c);
return( new_t );
    }

    t_mmax = e->up?e->tmax:e->tmin;
    t_mmin = e->up?e->tmin:e->tmax;
    /* sought_m += el->low; */

    while ( 1 ) {
	new_t = (t_mmin+t_mmax)/2;
	found_m = ( ((msp->a*new_t+msp->b)*new_t+msp->c)*new_t + msp->d );
	if ( found_m>sought_m-.001 && found_m<sought_m+.001 )
return( new_t );
	if ( found_m > sought_m ) {
	    t_mmax = new_t;
	} else {
	    t_mmin = new_t;
	}
	if ( t_mmax==t_mmin ) {
	    IError("EITOfNextMajor failed! on %s", el->sc!=NULL?el->sc->name:"Unknown" );
return( new_t );
	}
    }
}

EI *EIActiveListReorder(EI *active,int *change) {
    int any;
    EI *pr, *apt;

    *change = false;
    if ( active!=NULL ) {
	any = true;
	while ( any ) {
	    any = false;
	    for ( pr=NULL, apt=active; apt->aenext!=NULL; ) {
		if ( apt->ocur <= apt->aenext->ocur ) {
		    /* still ordered */;
		    pr = apt;
		    apt = apt->aenext;
		} else if ( pr==NULL ) {
		    active = apt->aenext;
		    apt->aenext = apt->aenext->aenext;
		    active->aenext = apt;
		    *change = true;
		    /* don't need to set any, since this reorder can't disorder the list */
		    pr = active;
		} else {
		    pr->aenext = apt->aenext;
		    apt->aenext = apt->aenext->aenext;
		    pr->aenext->aenext = apt;
		    any = *change = true;
		    pr = pr->aenext;
		}
	    }
	}
    }
return( active );
}

EI *EIActiveEdgesRefigure(EIList *el, EI *active,real i,int major, int *_change) {
    EI *apt, *pr, *npt;
    int change = false, subchange;
    int other = !major;

    /* first remove any entry which doesn't intersect the new scan line */
    /*  (ie. stopped on last line) */
    for ( pr=NULL, apt=active; apt!=NULL; apt = apt->aenext ) {
	if ( apt->coordmax[major]<i+el->low ) {
	    if ( pr==NULL )
		active = apt->aenext;
	    else
		pr->aenext = apt->aenext;
	    change = true;
	} else
	    pr = apt;
    }
    /* then move the active list to the next line */
    for ( apt=active; apt!=NULL; apt = apt->aenext ) {
	Spline1D *osp = &apt->spline->splines[other];
	apt->tcur = EITOfNextMajor(apt,el,i+el->low);
	apt->ocur = ( ((osp->a*apt->tcur+osp->b)*apt->tcur+osp->c)*apt->tcur + osp->d );
    }
    /* reorder list */
    active = EIActiveListReorder(active,&subchange);
    if ( subchange ) change = true;

    /* Insert new nodes */
    if ( el->ordered[(int) i]!=NULL ) change = true;
    for ( pr=NULL, apt=active, npt=el->ordered[(int) i]; apt!=NULL && npt!=NULL; ) {
	if ( npt->ocur<apt->ocur ) {
	    npt->aenext = apt;
	    if ( pr==NULL )
		active = npt;
	    else
		pr->aenext = npt;
	    pr = npt;
	    npt = npt->ordered;
	} else {
	    pr = apt;
	    apt = apt->aenext;
	}
    }
    while ( npt!=NULL ) {
	npt->aenext = NULL;
	if ( pr==NULL )
	    active = npt;
	else
	    pr->aenext = npt;
	pr = npt;
	npt = npt->ordered;
    }
    *_change = change;
return( active );
}


int EISkipExtremum(EI *e, real i, int major) {
    EI *n = e->aenext, *t;

    if ( n==NULL )
return( false );
    if ( 
	    (ceil(e->coordmin[major])==i || floor(e->coordmin[major])==i || floor(e->coordmax[major])==i || ceil(e->coordmax[major])==i) &&
	    (ceil(n->coordmin[major])==i || floor(n->coordmin[major])==i || floor(n->coordmax[major])==i || ceil(n->coordmax[major])==i) ) {
	if (
	     (n==e->splinenext && n->tmin==e->tmax &&
		    n->tcur<n->tmin+.2 && e->tcur>e->tmax-.2 ) ||
	     (n->splinenext==e && n->tmax==e->tmin &&
		    n->tcur>n->tmax-.2 && e->tcur<e->tmin+.2 ) )
return( n->up!=e->up );
	/* can be separated by a horizontal/vertical line in the other direction */
	if ( n->tmax==1 && e->tmin==0 && n->tcur>.8 && e->tcur<.2) {
	    t = n;
	    while ( (t = t->splinenext)!=e ) {
		if ( t==NULL || t==n ||
			(major && !t->hor) || ( !major && !t->vert ))
return( false );
	    }
return( n->up!=e->up );
	} else if ( n->tmin==0 && e->tmax==1 && n->tcur<.2 && e->tcur>.8) {
	    t = e;
	    while ( (t = t->splinenext)!=n ) {
		if ( t==NULL || t==e ||
			(major && !t->hor) || ( !major && !t->vert ))
return( false );
	    }
return( n->up!=e->up );
	}
    }
return( false );
}


real HIlen( StemInfo *stems) {
    HintInstance *hi;
    real len = 0;

    for ( hi=stems->where; hi!=NULL; hi = hi->next )
	len += hi->end-hi->begin;
return( len );
}

real HIoverlap( HintInstance *mhi, HintInstance *thi) {
    HintInstance *hi;
    real len = 0;
    real s, e;

    for ( ; mhi!=NULL; mhi = mhi->next ) {
	for ( hi = thi; hi!=NULL && hi->begin<=mhi->end; hi = hi->next ) {
	    if ( hi->end<mhi->begin ) {
		thi = hi;
	continue;
	    }
	    s = hi->begin<mhi->begin?mhi->begin:hi->begin;
	    e = hi->end>mhi->end?mhi->end:hi->end;
	    if ( e<s )
	continue;		/* Shouldn't happen */
	    len += e-s;
	}
    }
return( len );
}


int StemListAnyConflicts(StemInfo *stems) {
    StemInfo *s;
    int any= false;
    double end;

    for ( s=stems; s!=NULL ; s=s->next )
	s->hasconflicts = false;
    while ( stems!=NULL ) {
	end = stems->width<0 ? stems->start : stems->start+stems->width;
	for ( s=stems->next; s!=NULL && (s->width>0 ? s->start : s->start+s->width)<end; s=s->next ) {
	    stems->hasconflicts = true;
	    s->hasconflicts = true;
	    any = true;
	}
	stems = stems->next;
    }
return( any );
}

static HintInstance *SCGuessHintPoints(SplineChar *sc, int layer, StemInfo *stem,int major, int off) {
    SplinePoint *starts[20], *ends[20];
    int spt=0, ept=0;
    SplinePointList *spl;
    SplinePoint *sp, *np;
    int sm, wm, i, j, val;
    real coord;
    HintInstance *head, *test, *cur, *prev;

    for ( spl=sc->layers[layer].splines; spl!=NULL; spl=spl->next ) {
	for ( sp=spl->first; ; sp = np ) {
	    coord = (major?sp->me.x:sp->me.y);
	    sm = coord>=stem->start-off && coord<=stem->start+off;
	    wm = coord>=stem->start+stem->width-off && coord<=stem->start+stem->width+off;
	    if ( sm && spt<20 )
		starts[spt++] = sp;
	    if ( wm && ept<20 )
		ends[ept++] = sp;
	    if ( sp->next==NULL )
	break;
	    np = sp->next->to;
	    if ( np==spl->first )
	break;
	}
    }

    head = NULL;
    for ( i=0; i<spt; ++i ) {
	val = 0x80000000;
	for ( j=0; j<ept; ++j ) {
	    if ( major && starts[i]->me.y>=ends[j]->me.y-1 && starts[i]->me.y<=ends[j]->me.y+1 ) {
		val = starts[i]->me.y;
	break;
	    } else if ( !major && starts[i]->me.x>=ends[j]->me.x-1 && starts[i]->me.x<=ends[j]->me.x+1 ) {
		val = starts[i]->me.x;
	break;
	    }
	}
	if ( (unsigned)val!=0x80000000 ) {
	    for ( prev=NULL, test=head; test!=NULL && val>test->begin; prev=test, test=test->next );
	    if ( test==NULL || val!=test->begin ) {
		cur = chunkalloc(sizeof(HintInstance));
		cur->begin = cur->end = val;
		cur->next = test;
		if ( prev==NULL ) head = cur;
		else prev->next = cur;
	    }
	}
    }
return( head );
}

static void SCGuessHintInstancesLight(SplineChar *sc, int layer, StemInfo *stem,int major) {
    SplinePointList *spl;
    SplinePoint *sp, *np;
    int sm, wm, off;
    real ob, oe;
    HintInstance *s=NULL, *w=NULL, *cur, *p, *t, *n, *w2;
    /* We've got a hint (from somewhere, old data, reading in a font, user specified etc.) */
    /*  but we don't have HintInstance info. So see if we can find those data */
    /* Will get confused by stems with holes in them (for example if you make */
    /*  a hint from one side of an H to the other, it will get the whole thing */
    /*  not just the cross stem) */

    for ( spl=sc->layers[layer].splines; spl!=NULL; spl=spl->next ) {
	for ( sp=spl->first; ; sp = np ) {
	    sm = (major?sp->me.x:sp->me.y)==stem->start;
	    wm = (major?sp->me.x:sp->me.y)==stem->start+stem->width;
	    if ( sp->next==NULL )
	break;
	    np = sp->next->to;
	    if ( sm || wm ) {
		if ( !major ) {
		    if ( np->me.y==sp->me.y ) {
			ob = sp->me.x; oe = np->me.x;
		    } else if ( sp->nextcp.y==sp->me.y ) {
			ob = sp->me.x; oe = (sp->me.x+sp->nextcp.x)/2;
			if ( sp->prevcp.y==sp->me.y )
			    ob = (sp->prevcp.x+sp->me.x)/2;
		    } else if ( sp->prevcp.y==sp->me.y ) {
			ob = sp->me.x; oe = (sp->prevcp.x+sp->me.x)/2;
		    } else
			sm = wm = false;
		} else {
		    if ( np->me.x==sp->me.x ) {
			ob = sp->me.y; oe = np->me.y;
		    } else if ( sp->nextcp.x==sp->me.x ) {
			ob = sp->me.y; oe = (sp->nextcp.y+sp->me.y)/2;
			if ( sp->prevcp.x==sp->me.x )
			    ob = (sp->prevcp.y+sp->me.y)/2;
		    } else if ( sp->prevcp.x==sp->me.x ) {
			ob = sp->me.y; oe = (sp->prevcp.y+sp->me.y)/2;
		    } else
			sm = wm = false;
		}
	    }
	    if ( sm || wm ) {
		cur = chunkalloc(sizeof(HintInstance));
		if ( ob>oe ) { real temp=ob; ob=oe; oe=temp;}
		cur->begin = ob;
		cur->end = oe;
		if ( sm ) {
		    if ( s==NULL || s->begin>cur->begin ) {
			cur->next = s;
			s = cur;
		    } else {
			p = s;
			for ( t=s->next; t!=NULL && t->begin<cur->begin; p=t, t=t->next );
			p->next = cur; cur->next = t;
		    }
		} else {
		    if ( w==NULL || w->begin>cur->begin ) {
			cur->next = w;
			w = cur;
		    } else {
			p = w;
			for ( t=w->next; t!=NULL && t->begin<cur->begin; p=t, t=t->next );
			p->next = cur; cur->next = t;
		    }
		}
	    }
	    if ( np==spl->first )
	break;
	}
    }

    /* Now we know what the right side of the stem looks like, and we know */
    /*  what the left side looks like. They may not look the same (H for example) */
    /*  Figure out the set where both are active */
    /* Unless it's a ghost hint */
    if ( stem->width==20 && s==NULL && w!=NULL ) {
	s = w;
	w = NULL;
    } else if ( stem->width==21 && s!=NULL && w==NULL) {
	/* Just use s */;
    } else for ( p=NULL, t=s; t!=NULL; t=n ) {
	n = t->next;
	for ( w2=w; w2!=NULL && w2->begin<t->end ; w2=w2->next ) {
	    if ( w2->end<=t->begin )
	continue;
	    if ( w2->begin<=t->begin && w2->end>=t->end ) {
		/* Perfect match */
	break;
	    }
	    if ( w2->begin>=t->begin )
		t->begin = w2->begin;
	    if ( w2->end<=t->end ) {
		cur = chunkalloc(sizeof(HintInstance));
		cur->begin = w2->end;
		cur->end = t->end;
		cur->next = n;
		t->next = cur;
		n = cur;
		t->end = w2->end;
	    }
	break;
	}
	if ( w2!=NULL && w2->begin>=t->end )
	    w2 = NULL;
	if ( w2==NULL && w!=NULL ) {
	    HintInstance *best = NULL;
	    double best_off=1e10, off;
	    for ( w2=w; w2!=NULL ; w2=w2->next ) {
		if ( w2->end<=t->begin )
		    off = t->begin - w2->end;
		else
		    off = w2->begin - t->end;
		if ( best==NULL && off<best_off ) {
		    best = w2;
		    best_off = off;
		}
	    }
	    if ( best!=NULL && best_off<stem->width ) {
		w2 = best;
		if( w2->begin<t->begin )
		    t->begin = w2->begin;
		if ( w2->end>t->end )
		    t->end = w2->end;
	    }
	}
	if ( w2==NULL ) {
	    /* No match for t (or if there were it wasn't complete) get rid */
	    /*  of what's left of t */
	    chunkfree(t,sizeof(*t));
	    if ( p==NULL )
		s = n;
	    else
		p->next = n;
	} else
	    p = t;
    }
    while ( w!=NULL ) {
	n = w->next;
	chunkfree(w,sizeof(*w));
	w=n;
    }

    /* If we couldn't find anything, then see if there are two points which */
    /*  have the same x or y value and whose other coordinates match those of */
    /*  the hint */
    /* Surprisingly many fonts have hints which don't accurately match the */
    /*  points. Perhaps BlueFuzz (default 1) applies here too */
    for ( off=0; off<1 && s==NULL; ++off )
	s = SCGuessHintPoints(sc,layer, stem,major,off);

    stem->where = s;
}

void SCGuessHHintInstancesList(SplineChar *sc,int layer) {
    StemInfo *h;
    for ( h= sc->hstem; h!=NULL; h=h->next ) {
	if ( h->where==NULL ) {
	    SCGuessHintInstancesLight( sc,layer,h,false );
	}
    }
}

void SCGuessVHintInstancesList(SplineChar *sc,int layer) {
    StemInfo *h;
    for ( h= sc->vstem; h!=NULL; h=h->next ) {
	if ( h->where==NULL ) {
	    SCGuessHintInstancesLight( sc,layer,h,true );
	}
    }
}


static void _SCClearHintMasks(SplineChar *sc,int layer, int counterstoo) {
    SplineSet *spl;
    SplinePoint *sp;
    RefChar *ref;

    if ( counterstoo ) {
	free(sc->countermasks);
	sc->countermasks = NULL; sc->countermask_cnt = 0;
    }

    for ( spl = sc->layers[layer].splines; spl!=NULL; spl=spl->next ) {
	for ( sp = spl->first ; ; ) {
	    chunkfree(sp->hintmask,sizeof(HintMask));
	    sp->hintmask = NULL;
	    if ( sp->next==NULL )
	break;
	    sp = sp->next->to;
	    if ( sp==spl->first )
	break;
	}
    }

    for ( ref = sc->layers[layer].refs; ref!=NULL; ref=ref->next ) {
	for ( spl = ref->layers[0].splines; spl!=NULL; spl=spl->next ) {
	    for ( sp = spl->first ; ; ) {
		chunkfree(sp->hintmask,sizeof(HintMask));
		sp->hintmask = NULL;
		if ( sp->next==NULL )
	    break;
		sp = sp->next->to;
		if ( sp==spl->first )
	    break;
	    }
	}
    }
}


static void SCFigureSimpleCounterMasks(SplineChar *sc) {
    SplineChar *scs[MmMax];
    int hadh3, hadv3, i, vbase;
    HintMask mask;
    StemInfo *h;

    if ( sc->countermask_cnt!=0 )
return;

    scs[0] = sc;
    hadh3 = CvtPsStem3(NULL,scs,1,true,false);
    hadv3 = CvtPsStem3(NULL,scs,1,false,false);
    if ( hadh3 || hadv3 ) {
	memset(mask,0,sizeof(mask));
	if ( hadh3 ) mask[0] = 0x80|0x40|0x20;
	if ( hadv3 ) {
	    for ( h=sc->hstem, vbase=0; h!=NULL; h=h->next, ++vbase );
	    for ( i=0; i<3 ; ++i ) {
		int j = i+vbase;
		mask[j>>3] |= (0x80>>(j&7));
	    }
	}
	sc->countermask_cnt = 1;
	sc->countermasks = galloc(sizeof(HintMask));
	memcpy(sc->countermasks[0],mask,sizeof(HintMask));
return;
    }
}

void SCClearHintMasks(SplineChar *sc,int layer,int counterstoo) {
    MMSet *mm = sc->parent->mm;
    int i;

    if ( mm==NULL )
	_SCClearHintMasks(sc,layer,counterstoo);
    else {
	for ( i=0; i<mm->instance_count; ++i ) {
	    if ( sc->orig_pos<mm->instances[i]->glyphcnt )
		_SCClearHintMasks(mm->instances[i]->glyphs[sc->orig_pos],layer,counterstoo);
	}
	if ( sc->orig_pos<mm->normal->glyphcnt )
	    _SCClearHintMasks(mm->normal->glyphs[sc->orig_pos],layer,counterstoo);
    }
}

static StemInfo *OnHHint(SplinePoint *sp, StemInfo *s) {
    StemInfo *possible=NULL;
    HintInstance *hi;

    if ( sp==NULL )
return( NULL );

    for ( ; s!=NULL; s=s->next ) {
	if ( sp->me.y<s->start )
return( possible );
	if ( s->start==sp->me.y || s->start+s->width==sp->me.y ) {
	    if ( !s->hasconflicts )
return( s );
	    for ( hi=s->where; hi!=NULL; hi=hi->next ) {
		if ( hi->begin<=sp->me.x && hi->end>=sp->me.x )
return( s );
	    }
	    if ( !s->used )
		possible = s;
	}
    }
return( possible );
}

static StemInfo *OnVHint(SplinePoint *sp, StemInfo *s) {
    StemInfo *possible=NULL;
    HintInstance *hi;

    if ( sp==NULL )
return( NULL );

    for ( ; s!=NULL; s=s->next ) {
	if ( sp->me.x<s->start )
return( possible );
	if ( s->start==sp->me.x || s->start+s->width==sp->me.x ) {
	    if ( !s->hasconflicts )
return( s );
	    for ( hi=s->where; hi!=NULL; hi=hi->next ) {
		if ( hi->begin<=sp->me.y && hi->end>=sp->me.y )
return( s );
	    }
	    if ( !s->used )
		possible = s;
	}
    }
return( possible );
}

/* Does h have a conflict with any of the stems in the list which have bits */
/*  set in the mask */
static int ConflictsWithMask(StemInfo *stems, HintMask mask,StemInfo *h) {
    while ( stems!=NULL && stems->start<h->start+h->width ) {
	if ( stems->start+stems->width>=h->start && stems!=h ) {
	    if ( stems->hintnumber!=-1 &&
		    (mask[stems->hintnumber>>3]&(0x80>>(stems->hintnumber&7))) )
return( true );
	}
	stems = stems->next;
    }
return( false );
}

/* All instances of a MM set must have the same hint mask at all points */
static void FigureHintMask(SplineChar *scs[MmMax], SplinePoint *to[MmMax], int instance_count,
	HintMask mask) {
    StemInfo *s;
    int i;
    SplinePoint *sp;

    memset(mask,'\0',sizeof(HintMask));

    /* Install all hints that are always active */
    i=0; {
	SplineChar *sc = scs[i];

	if ( sc==NULL )
return;

	for ( s=sc->hstem; s!=NULL; s=s->next )
	    if ( s->hintnumber!=-1 && !s->hasconflicts )
		mask[s->hintnumber>>3] |= (0x80>>(s->hintnumber&7));
	for ( s=sc->vstem; s!=NULL; s=s->next )
	    if ( s->hintnumber!=-1 && !s->hasconflicts )
		mask[s->hintnumber>>3] |= (0x80>>(s->hintnumber&7));

	if ( sc->hconflicts ) {
	    for ( sp=to[i]; sp!=NULL; ) {
		s = OnHHint(sp,sc->hstem);
		if ( s!=NULL && s->hintnumber!=-1 ) {
		    if ( ConflictsWithMask(scs[i]->hstem,mask,s))
	    break;
		    mask[s->hintnumber>>3] |= (0x80>>(s->hintnumber&7));
		}
		if ( sp->next==NULL )
	    break;
		sp = sp->next->to;
		if ( to[i]==sp )
	    break;
	    }
	}
	if ( sc->vconflicts ) {
	    for ( sp=to[i]; sp!=NULL; ) {
		s = OnVHint(sp,sc->vstem);
		if ( s!=NULL && s->hintnumber!=-1 ) {
		    if ( ConflictsWithMask(scs[i]->vstem,mask,s))
	    break;
		    mask[s->hintnumber>>3] |= (0x80>>(s->hintnumber&7));
		}
		if ( sp->next==NULL )
	    break;
		sp = sp->next->to;
		if ( to[i]==sp )
	    break;
	    }
	}
    }
    for ( i=0; i<instance_count; ++i ) if ( to[i]!=NULL ) {
	chunkfree(to[i]->hintmask,sizeof(HintMask));
	to[i]->hintmask = chunkalloc(sizeof(HintMask));
	memcpy(to[i]->hintmask,mask,sizeof(HintMask));
    }
}

static int TestHintMask(SplineChar *scs[MmMax], SplinePoint *to[MmMax], int instance_count,
	HintMask mask) {
    StemInfo *h=NULL, *v=NULL;
    int i;

    for ( i=0; i<instance_count; ++i ) {
	SplineChar *sc = scs[i];

	if ( sc==NULL || (!sc->hconflicts && !sc->vconflicts ))
    continue;

	/* Does this point lie on any hints? */
	if ( scs[i]->hconflicts )
	    h = OnHHint(to[i],sc->hstem);
	if ( scs[i]->vconflicts )
	    v = OnVHint(to[i],sc->vstem);

	/* Need to set this hint */
	if ( (h!=NULL && h->hintnumber!=-1 && (mask[h->hintnumber>>3]&(0x80>>(h->hintnumber&7)))==0 ) ||
	     (v!=NULL && v->hintnumber!=-1 && (mask[v->hintnumber>>3]&(0x80>>(v->hintnumber&7)))==0 ))
    break;
    }
    if ( i==instance_count )	/* All hint masks were ok */
return( false );

    FigureHintMask(scs,to,instance_count,mask);
return( true );
}

static void UnnumberHints(SplineChar *sc) {
    StemInfo *h;

    for ( h=sc->hstem; h!=NULL; h=h->next )
	h->hintnumber = -1;
    for ( h=sc->vstem; h!=NULL; h=h->next )
	h->hintnumber = -1;
}

static int NumberHints(SplineChar *sc) {
    StemInfo *h;
    int hcnt=0;

    for ( h=sc->hstem; h!=NULL; h=h->next )
	h->hintnumber = hcnt>=HntMax ? -1 : hcnt++;
    for ( h=sc->vstem; h!=NULL; h=h->next )
	h->hintnumber = hcnt>=HntMax ? -1 : hcnt++;
return( hcnt );
}

static void UntickHints(SplineChar *sc) {
    StemInfo *h;

    for ( h=sc->hstem; h!=NULL; h=h->next )
	h->used = false;
    for ( h=sc->vstem; h!=NULL; h=h->next )
	h->used = false;
}

struct coords {
    real coords[MmMax];
    struct coords *next;
};

typedef struct mmh {
    StemInfo *hints[MmMax], *map[MmMax];
    struct coords *where;
    struct mmh *next;
} MMH;

static void AddCoord(MMH *mmh,SplinePoint *sps[MmMax],int instance_count, int ish) {
    struct coords *coords;
    int i;

    coords = chunkalloc(sizeof(struct coords));
    coords->next = mmh->where;
    mmh->where = coords;
    if ( ish )
	for ( i=0; i<instance_count; ++i )
	    coords->coords[i] = sps[i]->me.x;
    else
	for ( i=0; i<instance_count; ++i )
	    coords->coords[i] = sps[i]->me.y;
}

static MMH *AddHintSet(MMH *hints,StemInfo *h[MmMax], int instance_count,
	SplinePoint *sps[MmMax], int ish) {
    int i, cnt, bestc;
    MMH *test, *best;

    for ( i=0; i<instance_count; ++i )
	if ( h[i]==NULL )
return( hints );

    best = NULL; bestc = 0;
    for ( test=hints; test!=NULL; test=test->next ) {
	cnt = 0;
	for ( i=0; i<instance_count; ++i )
	    if ( test->hints[i]==h[i] )
		++cnt;
	if ( cnt==instance_count ) {
	    AddCoord(test,sps,instance_count,ish);
return( hints );
	}
	if ( cnt>bestc ) {
	    bestc = cnt;
	    best = test;
	}
    }
    test = chunkalloc(sizeof(MMH));
    test->next = hints;
    AddCoord(test,sps,instance_count,ish);
    for ( i=0; i<instance_count; ++i )
	test->hints[i]=h[i];
    if ( bestc!=0 ) {
	for ( i=0; i<instance_count; ++i ) {
	    if ( best->hints[i]==h[i] ) {
		h[i]->hasconflicts = true;
		test->map[i] = chunkalloc(sizeof(StemInfo));
		*test->map[i] = *h[i];
		test->map[i]->where = NULL;
		test->map[i]->used = true;
		h[i]->next = test->map[i];
	    } else
		test->map[i] = h[i];
	}
    } else {
	for ( i=0; i<instance_count; ++i )
	    test->map[i]=h[i];
    }
return( test );
}

static int CompareMMH(MMH *mmh1,MMH *mmh2, int instance_count) {
    int i;

    if ( mmh1->map[0]==NULL )
return( 1 );
    if ( mmh2->map[0]==NULL )
return( -1 );

    for ( i=0; i<instance_count; ++i ) {
	if ( mmh1->map[i]->start!=mmh2->map[i]->start ) {
	    if ( mmh1->map[i]->start > mmh2->map[i]->start )
return( 1 );
	    else
return( -1 );
	}
    }
    for ( i=0; i<instance_count; ++i ) {
	if ( mmh1->map[i]->width!=mmh2->map[i]->width ) {
	    if ( mmh1->map[i]->width > mmh2->map[i]->width )
return( 1 );
	    else
return( -1 );
	}
    }
return( 0 );
}
    
static MMH *SortMMH(MMH *head,int instance_count) {
    MMH *mmh, *p, *smallest, *psmallest, *test, *ptest;

    for ( mmh = head, p=NULL; mmh!=NULL ; ) {
	smallest = mmh; psmallest = p;
	ptest = mmh; test = mmh->next;
	while ( test!=NULL ) {
	    if ( CompareMMH(test,smallest,instance_count)<0 ) {
		smallest = test;
		psmallest = ptest;
	    }
	    ptest = test;
	    test = test->next;
	}
	if ( smallest!=mmh ) {
	    if ( p==NULL )
		head = smallest;
	    else
		p->next = smallest;
	    if ( mmh->next==smallest ) {
		mmh->next = smallest->next;
		smallest->next = mmh;
	    } else {
		test = mmh->next;
		mmh->next = smallest->next;
		smallest->next = test;
		psmallest->next = mmh;
	    }
	}
	p = smallest;
	mmh = smallest->next;
    }
return( head );
}

static int NumberMMH(MMH *mmh,int hstart,int instance_count) {
    int i;
    HintInstance *hi, *n;
    struct coords *coords;

    while ( mmh!=NULL ) {
	for ( i=0; i<instance_count; ++i ) {
	    StemInfo *h = mmh->map[i];
	    if ( h==NULL )
	continue;

	    h->hintnumber = hstart;

	    for ( hi=h->where; hi!=NULL; hi=n ) {
		n = hi->next;
		chunkfree(hi,sizeof(HintInstance));
	    }
	    h->where = NULL;
	    for ( coords=mmh->where; coords!=NULL; coords = coords->next ) {
		hi = chunkalloc(sizeof(HintInstance));
		hi->next = h->where;
		h->where = hi;
		hi->begin = coords->coords[i]-1;
		hi->end = coords->coords[i]+1;
	    }
	}
	if ( mmh->map[0]!=NULL ) ++hstart;
	mmh = mmh->next;
    }
return( hstart );
}
	    
static void SortMMH2(SplineChar *scs[MmMax],MMH *mmh,int instance_count,int ish) {
    int i;
    StemInfo *h, *n;
    MMH *m;

    for ( i=0; i<instance_count; ++i ) {
	for ( h= ish ? scs[i]->hstem : scs[i]->vstem; h!=NULL; h=n ) {
	    n = h->next;
	    if ( h->hintnumber==-1 )
		StemInfoFree(h);
	}
	n = NULL;
	for ( m = mmh ; m!=NULL; m=m->next ) {
	    h = m->map[i];
	    if ( n!=NULL )
		n->next = h;
	    else if ( ish )
		scs[i]->hstem = h;
	    else
		scs[i]->vstem = h;
	    n = h;
	}
	if ( n!=NULL )
	    n->next = NULL;
	else if ( ish )
	    scs[i]->hstem = NULL;
	else
	    scs[i]->vstem = NULL;
    }
}

static void MMHFreeList(MMH *mmh) {
    MMH *mn;
    struct coords *c, *n;

    for ( ; mmh!=NULL; mmh = mn ) {
	mn = mmh->next;
	for ( c=mmh->where; c!=NULL; c=n ) {
	    n = c->next;
	    chunkfree(c,sizeof(struct coords));
	}
	chunkfree(mmh,sizeof(struct coords));
    }
}

static void SplResolveSplitHints(SplineChar *scs[MmMax], SplineSet *spl[MmMax],
	int instance_count, MMH **hs, MMH **vs) {
    SplinePoint *to[MmMax];
    StemInfo *h[MmMax], *v[MmMax];
    int i, anymore;

    forever {
	for ( i=0; i<instance_count; ++i ) {
	    if ( spl[i]!=NULL )
		to[i] = spl[i]->first;
	    else
		to[i] = NULL;
	}
	forever {
	    for ( i=0; i<instance_count; ++i ) {
		h[i] = OnHHint(to[i],scs[i]->hstem);
		v[i] = OnVHint(to[i],scs[i]->vstem);
	    }
	    *hs = AddHintSet(*hs,h,instance_count,to,true);
	    *vs = AddHintSet(*vs,v,instance_count,to,false);
	    anymore = false;
	    for ( i=0; i<instance_count; ++i ) if ( to[i]!=NULL ) {
		if ( to[i]->next==NULL ) to[i] = NULL;
		else {
		    to[i] = to[i]->next->to;
		    if ( to[i]==spl[i]->first ) to[i] = NULL;
		}
		if ( to[i]!=NULL ) anymore = true;
	    }
	    if ( !anymore )
	break;
	}
	anymore = false;
	for ( i=0; i<instance_count; ++i ) {
	    if ( spl[i]!=NULL )
		spl[i] = spl[i]->next;
	    if ( spl[i]!=NULL ) anymore = true;
	}
	if ( !anymore )
    break;
    }
}

static void ResolveSplitHints(SplineChar *scs[16],int layer,int instance_count) {
    /* It is possible for a single hint in one mm instance to split into two */
    /*  in a different MM set. For example, we have two stems which happen */
    /*  to line up in one instance but which do not in another instance. */
    /* It is even possible that there could be no instance with any conflicts */
    /*  but some of the intermediate forms might conflict. */
    /* We can't deal (nor can postscript) with the case where hints change order*/
    SplinePointList *spl[MmMax];
    RefChar *ref[MmMax];
    int i, hcnt, hmax=0, anymore;
    MMH *hs=NULL, *vs=NULL;

    for ( i=0; i<instance_count; ++i ) {
	hcnt = NumberHints(scs[i]);
	UntickHints(scs[i]);
	if ( hcnt>hmax ) hmax = hcnt;
	spl[i] = scs[i]->layers[layer].splines;
    }
    if ( hmax==0 )
return;

    SplResolveSplitHints(scs,spl,instance_count,&hs,&vs);
    anymore = false;
    for ( i=0; i<instance_count; ++i ) {
	ref[i] = scs[i]->layers[layer].refs;
	if ( ref[i]!=NULL ) anymore = true;
    }
    while ( anymore ) {
	for ( i=0; i<instance_count; ++i )
	    spl[i] = ( ref[i]!=NULL ) ? ref[i]->layers[0].splines : NULL;
	SplResolveSplitHints(scs,spl,instance_count,&hs,&vs);
	anymore = false;
	for ( i=0; i<instance_count; ++i ) {
	    if ( ref[i]!=NULL ) {
		ref[i] = ref[i]->next;
		if ( ref[i]!=NULL ) anymore = true;
	    }
	}
    }

    for ( i=0; i<instance_count; ++i )
	UnnumberHints(scs[i]);
    hs = SortMMH(hs,instance_count);
    vs = SortMMH(vs,instance_count);
    hcnt = NumberMMH(hs,0,instance_count);
    hcnt = NumberMMH(vs,hcnt,instance_count);
    SortMMH2(scs,hs,instance_count,true);
    SortMMH2(scs,vs,instance_count,false);
    MMHFreeList(hs);
    MMHFreeList(vs);
}

static int SplFigureHintMasks(SplineChar *scs[MmMax], SplineSet *spl[MmMax],
	int instance_count, HintMask mask, int inited) {
    SplinePoint *to[MmMax];
    int i, anymore;

    anymore = false;
    for ( i=0; i<instance_count; ++i ) {
	if ( spl[i]!=NULL ) {
	    SplineSetReverse(spl[i]);
	    to[i] = spl[i]->first;
	    anymore = true;
	} else
	    to[i] = NULL;
    }

    /* Assign the initial hint mask */
    if ( anymore && !inited ) {
	FigureHintMask(scs,to,instance_count,mask);
	inited = true;
    }

    forever {
	for ( i=0; i<instance_count; ++i ) {
	    if ( spl[i]!=NULL )
		to[i] = spl[i]->first;
	    else
		to[i] = NULL;
	}
	forever {
	    TestHintMask(scs,to,instance_count,mask);
	    anymore = false;
	    for ( i=0; i<instance_count; ++i ) if ( to[i]!=NULL ) {
		if ( to[i]->next==NULL ) to[i] = NULL;
		else {
		    to[i] = to[i]->next->to;
		    if ( to[i]==spl[i]->first ) to[i] = NULL;
		}
		if ( to[i]!=NULL ) anymore = true;
	    }
	    if ( !anymore )
	break;
	}
	anymore = false;
	for ( i=0; i<instance_count; ++i ) {
	    if ( spl[i]!=NULL ) {
		SplineSetReverse(spl[i]);
		spl[i] = spl[i]->next;
	    }
	    if ( spl[i]!=NULL ) {
		anymore = true;
		SplineSetReverse(spl[i]);
	    }
	}
	if ( !anymore )
    break;
    }
return( inited );
}

void SCFigureHintMasks(SplineChar *sc,int layer) {
    SplineChar *scs[MmMax];
    SplinePointList *spl[MmMax];
    RefChar *ref[MmMax];
    MMSet *mm = sc->parent->mm;
    int i, instance_count, conflicts, anymore, inited;
    HintMask mask;

    if ( mm==NULL ) {
	scs[0] = sc;
	instance_count = 1;
	SCClearHintMasks(sc,layer,false);
    } else {
	if ( mm->apple )
return;
	instance_count = mm->instance_count;
	for ( i=0; i<instance_count; ++i )
	    if ( sc->orig_pos < mm->instances[i]->glyphcnt ) {
		scs[i] = mm->instances[i]->glyphs[sc->orig_pos];
		SCClearHintMasks(scs[i],layer,false);
	    }
	ResolveSplitHints(scs,layer,instance_count);
    }
    conflicts = false;
    for ( i=0; i<instance_count; ++i ) {
	NumberHints(scs[i]);
	if ( scs[i]->hconflicts || scs[i]->vconflicts )
	    conflicts = true;
    }
    if ( !conflicts && instance_count==1 ) {	/* All hints always active */
	SCFigureSimpleCounterMasks(sc);
return;						/* In an MM font we may still need to resolve things like different numbers of hints */
    }

    for ( i=0; i<instance_count; ++i ) {
	spl[i] = scs[i]->layers[layer].splines;
	ref[i] = scs[i]->layers[layer].refs;
    }
    inited = SplFigureHintMasks(scs,spl,instance_count,mask,false);
    forever {
	for ( i=0; i<instance_count; ++i ) {
	    if ( ref[i]!=NULL )
		spl[i] = ref[i]->layers[0].splines;
	}
	inited = SplFigureHintMasks(scs,spl,instance_count,mask,inited);
	anymore = false;
	for ( i=0; i<instance_count; ++i ) {
	    if ( ref[i]!=NULL ) {
		ref[i] = ref[i]->next;
		if ( ref[i]!=NULL ) anymore = true;
	    }
	}
	if ( !anymore )
    break;
    }
    if ( instance_count==1 )
	SCFigureSimpleCounterMasks(sc);
}



static void FigureStems( SplineFont *sf, real snaps[12], real cnts[12],
	int which ) {
    int i, j, k, cnt, smax=0, smin=2000;
    real stemwidths[2000];
    StemInfo *stems, *test;
    int len;
    HintInstance *hi;

    memset(stemwidths,'\0',sizeof(stemwidths));

    for ( i=0; i<sf->glyphcnt; ++i ) if ( sf->glyphs[i]!=NULL ) {
	stems = which?sf->glyphs[i]->hstem:sf->glyphs[i]->vstem;
	for ( test=stems; test!=NULL; test = test->next ) if ( !test->ghost ) {
	    if ( (j=test->width)<0 ) j= -j;
	    if ( j<2000 ) {
		len = 0;
		for ( hi=test->where; hi!=NULL; hi=hi->next )
		    len += hi->end-hi->begin;
		if ( len==0 ) len = 100;
		stemwidths[j] += len;
		if ( smax<j ) smax=j;
		if ( smin>j ) smin=j;
	    }
	}
    }

    for ( i=smin, cnt=0; i<=smax; ++i ) {
	if ( stemwidths[i]!=0 )
	    ++cnt;
    }

    if ( cnt>12 ) {
	/* Merge width windows */
	int windsize=3, j;
	for ( i=smin; i<=smax; ++i ) if ( stemwidths[i]!=0 ) {
	    if ( (j = i-windsize)<0 ) j=0;
	    for ( ; j<smax && j<=i+windsize; ++j )
		if ( stemwidths[i]<stemwidths[j] )
	    break;
	    if ( j==smax || j>i+windsize ) {
		if ( (j = i-windsize)<0 ) j=0;
		for ( ; j<smax && j<=i+windsize; ++j ) if ( j!=i ) {
		    stemwidths[i] += stemwidths[j];
		    stemwidths[j] = 0;
		}
	    }
	}
	/* Merge adjacent widths */
	for ( i=smin; i<=smax; ++i ) {
	    if ( stemwidths[i]!=0 && i<=smax-1 && stemwidths[i+1]!=0 ) {
		if ( stemwidths[i]>stemwidths[i+1] ) {
		    stemwidths[i] += stemwidths[i+1];
		    stemwidths[i+1] = 0;
		} else {
		    if ( i<=smax-2 && stemwidths[i+2] && stemwidths[i+2]<stemwidths[i+1] ) {
			stemwidths[i+1] += stemwidths[i+2];
			stemwidths[i+2] = 0;
		    }
		    stemwidths[i+1] += stemwidths[i];
		    stemwidths[i] = 0;
		    ++i;
		}
	    }
	}
	for ( i=smin, cnt=0; i<=smax; ++i ) {
	    if ( stemwidths[i]!=0 )
		++cnt;
	}
    }
    if ( cnt<=12 ) {
	for ( i=smin, cnt=0; i<=smax; ++i ) {
	    if ( stemwidths[i]!=0 ) {
		snaps[cnt] = i;
		cnts[cnt++] = stemwidths[i];
	    }
	}
    } else { real firstbiggest=0;
	for ( cnt = 0; cnt<12; ++cnt ) {
	    int biggesti=0;
	    real biggest=0;
	    for ( i=smin; i<=smax; ++i ) {
		if ( stemwidths[i]>biggest ) { biggest = stemwidths[i]; biggesti=i; }
	    }
	    /* array must be sorted */
	    if ( biggest<firstbiggest/6 )
	break;
	    for ( j=0; j<cnt; ++j )
		if ( snaps[j]>biggesti )
	    break;
	    for ( k=cnt-1; k>=j; --k ) {
		snaps[k+1] = snaps[k];
		cnts[k+1]=cnts[k];
	    }
	    snaps[j] = biggesti;
	    cnts[j] = biggest;
	    stemwidths[biggesti] = 0;
	    if ( firstbiggest==0 ) firstbiggest = biggest;
	}
    }
    for ( ; cnt<12; ++cnt ) {
	snaps[cnt] = 0;
	cnts[cnt] = 0;
    }
}

void FindHStems( SplineFont *sf, real snaps[12], real cnt[12]) {
    FigureStems(sf,snaps,cnt,1);
}

void FindVStems( SplineFont *sf, real snaps[12], real cnt[12]) {
    FigureStems(sf,snaps,cnt,0);
}

static int IsFlexSmooth(SplinePoint *sp) {
    BasePoint nvec, pvec;
    double proj_same, proj_normal;

    if ( sp->nonextcp || sp->noprevcp )
return( false );	/* No continuity of slopes */

    nvec.x = sp->nextcp.x - sp->me.x; nvec.y = sp->nextcp.y - sp->me.y;
    pvec.x = sp->me.x - sp->prevcp.x; pvec.y = sp->me.y - sp->prevcp.y;

    /* Avoid cases where the slopes are 180 out of phase */
    if ( (proj_same   = nvec.x*pvec.x + nvec.y*pvec.y)<=0 )
return( false );
    if ( (proj_normal = nvec.x*pvec.y - nvec.y*pvec.x)<0 )
	proj_normal = -proj_normal;

    /* Something is smooth if the normal projection is 0. Let's allow for */
    /*  some rounding errors */
    if ( proj_same >= 16*proj_normal )
return( true );

return( false );
}

static int _SplineCharIsFlexible(SplineChar *sc, int layer, int blueshift) {
    /* Need two splines
	outer endpoints have same x (or y) values
	inner point must be less than 20 horizontal (v) units from the outer points
	inner point must also be less than BlueShift units (defaults to 7=>6)
	    (can increase BlueShift up to 21)
	the inner point must be a local extremum
	the inner point's cps must be at the x (or y) value as the extremum
		(I think)
    */
    /* We want long, nearly straight stems. If the end-points should not have
      continuous slopes, or if they do, they must be horizontal/vertical.
      This is an heuristic requirement, not part of Adobe's spec.
    */
    SplineSet *spl;
    SplinePoint *sp, *np, *pp;
    int max=0, val;
    RefChar *r;

    if ( sc==NULL )
return(false);

    for ( spl = sc->layers[layer].splines; spl!=NULL; spl=spl->next ) {
	if ( spl->first->prev==NULL ) {
	    /* Mark everything on the open path as inflexible */
	    sp=spl->first;
	    while ( 1 ) {
		sp->flexx = sp->flexy = false;
		if ( sp->next==NULL )
	    break;
		sp = sp->next->to;
	    } 
    continue;				/* Ignore open paths */
	}
	sp=spl->first;
	do {
	    if ( sp->next==NULL || sp->prev==NULL )
	break;
	    np = sp->next->to;
	    pp = sp->prev->from;
	    if ( !pp->flexx && !pp->flexy ) {
		sp->flexy = sp->flexx = 0;
		val = 0;
		if ( RealNear(sp->nextcp.x,sp->me.x) &&
			RealNear(sp->prevcp.x,sp->me.x) &&
			RealNear(np->me.x,pp->me.x) &&
			!RealNear(np->me.x,sp->me.x) &&
			(!IsFlexSmooth(pp) || RealNear(pp->nextcp.x,pp->me.x)) &&
			(!IsFlexSmooth(np) || RealNear(np->prevcp.x,np->me.x)) &&
			np->me.x-sp->me.x < blueshift &&
			np->me.x-sp->me.x > -blueshift ) {
		    if ( (np->me.x>sp->me.x &&
			    np->prevcp.x<=np->me.x && np->prevcp.x>=sp->me.x &&
			    pp->nextcp.x<=pp->me.x && pp->prevcp.x>=sp->me.x ) ||
			  (np->me.x<sp->me.x &&
			    np->prevcp.x>=np->me.x && np->prevcp.x<=sp->me.x &&
			    pp->nextcp.x>=pp->me.x && pp->prevcp.x<=sp->me.x )) {
			sp->flexx = true;
			val = np->me.x-sp->me.x;
		    }
		}
		if ( RealNear(sp->nextcp.y,sp->me.y) &&
			RealNear(sp->prevcp.y,sp->me.y) &&
			RealNear(np->me.y,pp->me.y) &&
			!RealNear(np->me.y,sp->me.y) &&
			(!IsFlexSmooth(pp) || RealNear(pp->nextcp.y,pp->me.y)) &&
			(!IsFlexSmooth(np) || RealNear(np->prevcp.y,np->me.y)) &&
			np->me.y-sp->me.y < blueshift &&
			np->me.y-sp->me.y > -blueshift ) {
		    if ( (np->me.y>sp->me.y &&
			    np->prevcp.y<=np->me.y && np->prevcp.y>=sp->me.y &&
			    pp->nextcp.y<=pp->me.y && pp->nextcp.y>=sp->me.y ) ||
			  (np->me.y<sp->me.y &&
			    np->prevcp.y>=np->me.y && np->prevcp.y<=sp->me.y &&
			    pp->nextcp.y>=pp->me.y && pp->nextcp.y<=sp->me.y )) {
			sp->flexy = true;
			val = np->me.y-sp->me.y;
		    }
		}
		if ( val<0 ) val = -val;
		if ( val>max ) max = val;
	    }
	    sp = np;
	} while ( sp!=spl->first );
    }
    sc->layers[layer].anyflexes = max>0;
    if ( max==0 )
	for ( r = sc->layers[layer].refs; r!=NULL ; r=r->next )
	    if ( r->sc->layers[layer].anyflexes ) {
		sc->layers[layer].anyflexes = true;
	break;
	    }
return( max );
}



static void SCUnflex(SplineChar *sc, int layer) {
    SplineSet *spl;
    SplinePoint *sp;

    for ( spl = sc->layers[layer].splines; spl!=NULL; spl=spl->next ) {
	/* Mark everything on the path as inflexible */
	sp=spl->first;
	while ( 1 ) {
	    sp->flexx = sp->flexy = false;
	    if ( sp->next==NULL )
	break;
	    sp = sp->next->to;
	    if ( sp==spl->first )
	break;
	} 
    }
    sc->layers[layer].anyflexes = false;
}

static void FlexDependents(SplineChar *sc,int layer) {
    struct splinecharlist *scl;

    sc->layers[layer].anyflexes = true;
    for ( scl = sc->dependents; scl!=NULL; scl=scl->next )
	FlexDependents(scl->sc,layer);
}

int SplineFontIsFlexible(SplineFont *sf,int layer, int flags) {
    int i;
    int max=0, val;
    char *pt;
    int blueshift;
    /* if the return value is bigger than 6 and we don't have a BlueShift */
    /*  then we must set BlueShift to ret+1 before saving private dictionary */
    /* If the first point in a spline set is flexible, then we must rotate */
    /*  the splineset */

    if ( flags&(ps_flag_nohints|ps_flag_noflex)) {
	for ( i=0; i<sf->glyphcnt; ++i ) if ( sf->glyphs[i]!=NULL )
	    SCUnflex(sf->glyphs[i],layer);
return( 0 );
    }
	
    pt = PSDictHasEntry(sf->private,"BlueShift");
    blueshift = 21;		/* maximum posible flex, not default */
    if ( pt!=NULL ) {
	blueshift = strtol(pt,NULL,10);
	if ( blueshift>21 ) blueshift = 21;
    } else if ( PSDictHasEntry(sf->private,"BlueValues")!=NULL )
	blueshift = 7;	/* The BlueValues array may depend on BlueShift having its default value */

    for ( i=0; i<sf->glyphcnt; ++i )
	if ( sf->glyphs[i]!=NULL ) if ( sf->glyphs[i]!=NULL ) {
	    val = _SplineCharIsFlexible(sf->glyphs[i],layer,blueshift);
	    if ( val>max ) max = val;
	    if ( sf->glyphs[i]->layers[layer].anyflexes )
		FlexDependents(sf->glyphs[i],layer);
	}
return( max );
}


