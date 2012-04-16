/* Header for module strings, generated by p2c 1.21alpha-07.Dec.93 */
#ifndef STRINGS_H
#define STRINGS_H


#ifdef STRINGS_G
# define vextern
#else
# define vextern extern
#endif
/* String handling primitives. */
/* These should be recoded in C instead of using the p2c code. */


extern Void scan1 PP((Char *s, int p, short *n));
/* Read an integer at position p of s */
extern boolean startsWith PP((Char *s1, Char *s2));
extern short pos1 PP((int c, Char *s));
extern short posNot PP((int c, Char *s));
extern Void insertChar PP((int c, Char *s, int p));
extern Char *substr_ PP((Char *Result, Char *s, int start, int count));
extern Void getNum PP((Char *line, short *k));
extern Void getTwoNums PP((Char *line, short *k1, short *k2));
extern Void toUpper PP((Char *s));
extern Void delete1 PP((Char *s, int p));
extern Void predelete PP((Char *s, int l));
extern Void shorten PP((Char *s, int new_length));


#undef vextern

#endif /*STRINGS_H*/

/* End. */
