/* Header for module preamble, generated by p2c 1.21alpha-07.Dec.93 */
#ifndef PREAMBLE_H
#define PREAMBLE_H


/*$X+*/
/* Interpret preamble paragraph, compose PMX preamble */

#ifndef GLOBALS_H
#include "globals.h"
#endif


#ifdef PREAMBLE_G
# define vextern
#else
# define vextern extern
#endif

extern boolean thisCase(void);
extern void preambleDefaults(void);
extern void interpretCommands(void);
extern void doPreamble(void);
extern void doPMXpreamble(void);
extern void respace(void);
extern void restyle(void);
extern Char *startString(Char *Result, voice_index0 voice);
extern void augmentPreamble(boolean control_para);
extern boolean omitLine(paragraph_index line);
extern void nonMusic(void);
extern void setOnly(Char *line);
extern boolean isCommand(Char *command);


#define known           true


#undef vextern

#endif /*PREAMBLE_H*/

/* End. */
