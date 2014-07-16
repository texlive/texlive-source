/* Header for module utility, generated by p2c 1.21alpha-07.Dec.93 */
#ifndef UTILITY_H
#define UTILITY_H


#ifdef UTILITY_G
# define vextern
#else
# define vextern extern
#endif
/* DPL 2004-03-22 */

/* Utilities, mainly aids to parsing */


extern boolean equalsIgnoreCase PP((Char *s1, Char *s2));
extern boolean startsWithIgnoreCase PP((Char *s1, Char *s2));
extern boolean endsWith PP((Char *s1, Char *s2));
extern boolean startsWithBracedWord PP((Char *P));
extern Char *GetNextWord PP((Char *Result, Char *s, int Delim, int Term));
extern Char *NextWord PP((Char *Result, Char *s, int Delim, int Term));
extern short wordCount PP((Char *s));
extern Char *plural PP((Char *Result, int n));
extern short curtail PP((Char *s, int c));
/* Remove last character if it equals c and return its position;
   otherwise return 0 */
extern Char *toString PP((Char *Result, int n));
extern Void trim PP((Char *s));
extern short digit PP((int c));
extern boolean match PP((Char *source, Char *pattern));
extern Char *translate PP((Char *Result, Char *source, Char *pattern,
			   Char *target));
extern Void grep PP((Char *source, Char *pattern, Char *target));
/* See Implementation for what this currently does. */


#undef vextern

#endif /*UTILITY_H*/

/* End. */