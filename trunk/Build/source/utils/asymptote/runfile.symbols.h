/*****
 * This file is automatically generated by findsym.pl
 * Changes will be overwritten.
 *****/

// If the ADDSYMBOL macro is not already defined, define it with the default
// purpose of referring to an external pre-translated symbol, such that
// SYM(name) also refers to that symbol.
#ifndef ADDSYMBOL
    #define ADDSYMBOL(name) extern sym::symbol PRETRANSLATED_SYMBOL_##name
    #ifdef PRESYM
        #define SYM(name) PRETRANSLATED_SYMBOL_##name
    #else
        #define SYM(name) sym::symbol::trans(#name)
    #endif
#endif

ADDSYMBOL(a);
ADDSYMBOL(b);
ADDSYMBOL(binput);
ADDSYMBOL(boutput);
ADDSYMBOL(check);
ADDSYMBOL(clear);
ADDSYMBOL(close);
ADDSYMBOL(comment);
ADDSYMBOL(delete);
ADDSYMBOL(digits);
ADDSYMBOL(eof);
ADDSYMBOL(eol);
ADDSYMBOL(error);
ADDSYMBOL(f);
ADDSYMBOL(flush);
ADDSYMBOL(from);
ADDSYMBOL(getc);
ADDSYMBOL(input);
ADDSYMBOL(name);
ADDSYMBOL(output);
ADDSYMBOL(pos);
ADDSYMBOL(precision);
ADDSYMBOL(rename);
ADDSYMBOL(s);
ADDSYMBOL(seek);
ADDSYMBOL(seekeof);
ADDSYMBOL(tell);
ADDSYMBOL(to);
ADDSYMBOL(update);
ADDSYMBOL(xinput);
ADDSYMBOL(xoutput);
