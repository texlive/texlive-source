/* web2c.h: general includes for the Web2c program itself.  Public domain.  */

#include "config.h"

#define ex_32 2
#define ex_real 3

#undef max
#define max(a,b) ((a>b)?a:b)

extern int indent;
extern int line_pos;
extern int last_brace;
extern int block_level;
extern int ii;
extern int last_tok;

extern char safe_string[80];
extern char var_list[200];
extern char field_list[200];
extern char last_id[80];
extern char z_id[80];
extern char next_temp[];

extern long last_i_num;
extern int ii, l_s;
extern long lower_bound, upper_bound;
extern FILE *out;
extern int pf_count;

/* A symbol table entry.  */
struct sym_entry {
  char *id;	/* points to the identifier */
  int typ;	/* token type */
  int next;	/* next symbol entry */
  long val;	/* constant : value
		   subrange type : lower bound */
  long upper; 	/* subrange type : upper bound
		   variable, function, type or field : type length */
  int val_sym, upper_sym;	/* Sym table entries of symbols for lower
				   and upper bounds
				 */
  boolean var_formal;	/* Is this a formal parameter by reference? */
  boolean var_not_needed;
  		      /* True if VAR token should be ignored for this type */
};

extern char strings[];
extern int hash_list[];
extern short global;
extern struct sym_entry sym_table[];
extern int next_sym_free, next_string_free;
extern int mark_sym_free, mark_string_free;

extern void find_next_temp (void);
extern void normal (void);
extern void new_line (void);
extern void my_output (const_string);
extern void semicolon (void);
extern void remove_locals (void);
extern void mark (void);
extern void initialize (void);
extern int add_to_table (string);
extern int search_table (const_string);
extern int yyerror (const_string);

extern void get_string_literal (char*);
extern void get_single_char (char*);
extern void get_result_type (char*);
extern void get_return_type (char*);

extern int yylex(void), yyparse(void);
