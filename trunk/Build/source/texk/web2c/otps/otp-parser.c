
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 25 "otp-parser.y"

#include "otp.h"
#include "routines.h"
#include "yystype.h"
int k, len;

void yyerror(char * msg)
{
fprintf(stderr, "line %d: %s\n", line_number, msg);
}


/* Line 189 of yacc.c  */
#line 86 "otp-parser.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NUMBER = 258,
     ID = 259,
     STRING = 260,
     LEFTARROW = 261,
     RIGHTARROW = 262,
     INPUT = 263,
     OUTPUT = 264,
     ALIASES = 265,
     STATES = 266,
     TABLES = 267,
     EXPRESSIONS = 268,
     PUSH = 269,
     POP = 270,
     DIV = 271,
     MOD = 272,
     BEG = 273,
     END = 274
   };
#endif
/* Tokens.  */
#define NUMBER 258
#define ID 259
#define STRING 260
#define LEFTARROW 261
#define RIGHTARROW 262
#define INPUT 263
#define OUTPUT 264
#define ALIASES 265
#define STATES 266
#define TABLES 267
#define EXPRESSIONS 268
#define PUSH 269
#define POP 270
#define DIV 271
#define MOD 272
#define BEG 273
#define END 274




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 166 "otp-parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   173

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  87
/* YYNRULES -- Number of states.  */
#define YYNSTATES  161

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   274

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    39,    38,     2,     2,     2,
      34,    35,    22,    20,    29,    21,    32,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    23,
      30,    26,    31,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    24,    37,    25,    33,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    27,    36,    28,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,    10,    11,    15,    16,    20,    21,    24,
      26,    29,    30,    41,    42,    44,    46,    50,    51,    55,
      57,    61,    62,    65,    67,    70,    75,    77,    84,    90,
      95,    97,    99,   103,   105,   110,   114,   118,   120,   124,
     127,   129,   132,   133,   134,   135,   146,   147,   150,   151,
     155,   159,   162,   163,   165,   166,   168,   170,   173,   174,
     177,   179,   181,   184,   187,   194,   197,   204,   211,   220,
     223,   225,   229,   233,   237,   241,   245,   246,   252,   254,
     257,   260,   267,   271,   272,   275,   279,   284
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      41,     0,    -1,    42,    43,    44,    50,    52,    58,    -1,
      -1,     8,     3,    23,    -1,    -1,     9,     3,    23,    -1,
      -1,    12,    45,    -1,    46,    -1,    45,    46,    -1,    -1,
       4,    24,     3,    25,    47,    26,    27,    48,    28,    23,
      -1,    -1,    49,    -1,     3,    -1,    49,    29,     3,    -1,
      -1,    11,    51,    23,    -1,     4,    -1,    51,    29,     4,
      -1,    -1,    10,    53,    -1,    54,    -1,    53,    54,    -1,
       4,    26,    55,    23,    -1,     5,    -1,    56,    30,     3,
      29,     3,    31,    -1,    56,    30,     3,    29,    31,    -1,
      56,    30,     3,    31,    -1,    56,    -1,     3,    -1,     3,
      21,     3,    -1,    32,    -1,    33,    34,    57,    35,    -1,
      34,    57,    35,    -1,    27,     4,    28,    -1,    56,    -1,
      57,    36,    56,    -1,    13,    59,    -1,    60,    -1,    59,
      60,    -1,    -1,    -1,    -1,    65,    61,    66,    62,     7,
      70,    63,    64,    75,    23,    -1,    -1,     6,    70,    -1,
      -1,    30,     4,    31,    -1,    67,    69,    68,    -1,    67,
      68,    -1,    -1,    18,    -1,    -1,    19,    -1,    55,    -1,
      69,    55,    -1,    -1,    70,    71,    -1,     5,    -1,     3,
      -1,    37,     3,    -1,    37,    38,    -1,    37,    34,    38,
      21,     3,    35,    -1,    37,    22,    -1,    37,    34,    22,
      20,     3,    35,    -1,    37,    34,    22,    21,     3,    35,
      -1,    37,    34,    22,    20,     3,    21,     3,    35,    -1,
      39,    74,    -1,    74,    -1,    72,    20,    74,    -1,    72,
      21,    74,    -1,    72,    22,    74,    -1,    72,    16,    74,
      -1,    72,    17,    74,    -1,    -1,     4,    73,    24,    72,
      25,    -1,     3,    -1,    37,     3,    -1,    37,    38,    -1,
      37,    34,    38,    21,     3,    35,    -1,    34,    72,    35,
      -1,    -1,    30,    31,    -1,    30,     4,    31,    -1,    30,
      14,     4,    31,    -1,    30,    15,    31,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    61,    61,    71,    72,    78,    79,    83,    85,    89,
      90,    95,    94,    99,   101,   105,   107,   111,   113,   117,
     119,   123,   125,   129,   130,   134,   139,   141,   143,   145,
     147,   152,   154,   156,   158,   160,   162,   167,   169,   174,
     189,   190,   195,   197,   199,   194,   204,   206,   211,   212,
     217,   219,   225,   226,   232,   233,   238,   240,   244,   246,
     250,   257,   259,   261,   263,   265,   270,   275,   280,   285,
     290,   291,   293,   295,   297,   299,   302,   301,   308,   310,
     312,   314,   316,   319,   321,   323,   325,   327
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUMBER", "ID", "STRING", "LEFTARROW",
  "RIGHTARROW", "INPUT", "OUTPUT", "ALIASES", "STATES", "TABLES",
  "EXPRESSIONS", "PUSH", "POP", "DIV", "MOD", "BEG", "END", "'+'", "'-'",
  "'*'", "';'", "'['", "']'", "'='", "'{'", "'}'", "','", "'<'", "'>'",
  "'.'", "'^'", "'('", "')'", "'|'", "'\\\\'", "'$'", "'#'", "$accept",
  "File", "Input", "Output", "Tables", "MoreTables", "OneTable", "$@1",
  "Numbers", "MoreNumbers", "States", "MoreStates", "Aliases",
  "MoreAliases", "OneAlias", "OneCompleteLeft", "OneLeft", "ChoiceLeft",
  "Expressions", "MoreExpressions", "OneExpr", "$@2", "$@3", "$@4",
  "PushBack", "LeftState", "TotalLeft", "BegLeft", "EndLeft", "Left",
  "Right", "OneRight", "RestRightExpr", "$@5", "OneRightExpr",
  "RightState", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      43,    45,    42,    59,    91,    93,    61,   123,   125,    44,
      60,    62,    46,    94,    40,    41,   124,    92,    36,    35
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    40,    41,    42,    42,    43,    43,    44,    44,    45,
      45,    47,    46,    48,    48,    49,    49,    50,    50,    51,
      51,    52,    52,    53,    53,    54,    55,    55,    55,    55,
      55,    56,    56,    56,    56,    56,    56,    57,    57,    58,
      59,    59,    61,    62,    63,    60,    64,    64,    65,    65,
      66,    66,    67,    67,    68,    68,    69,    69,    70,    70,
      71,    71,    71,    71,    71,    71,    71,    71,    71,    71,
      72,    72,    72,    72,    72,    72,    73,    72,    74,    74,
      74,    74,    74,    75,    75,    75,    75,    75
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     6,     0,     3,     0,     3,     0,     2,     1,
       2,     0,    10,     0,     1,     1,     3,     0,     3,     1,
       3,     0,     2,     1,     2,     4,     1,     6,     5,     4,
       1,     1,     3,     1,     4,     3,     3,     1,     3,     2,
       1,     2,     0,     0,     0,    10,     0,     2,     0,     3,
       3,     2,     0,     1,     0,     1,     1,     2,     0,     2,
       1,     1,     2,     2,     6,     2,     6,     6,     8,     2,
       1,     3,     3,     3,     3,     3,     0,     5,     1,     2,
       2,     6,     3,     0,     2,     3,     4,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     0,     5,     0,     1,     0,     7,     4,     0,
       0,    17,     6,     0,     8,     9,     0,    21,     0,    10,
      19,     0,     0,     0,     0,    18,     0,     0,    22,    23,
      48,     2,    11,    20,     0,    24,     0,    48,    40,    42,
       0,    31,    26,     0,    33,     0,     0,     0,    30,     0,
      41,    52,     0,     0,     0,     0,    37,     0,    25,     0,
      49,    53,    43,    54,    13,    32,    36,     0,    35,     0,
       0,     0,    55,    56,    51,    54,    15,     0,    14,    34,
      38,     0,    29,    58,    57,    50,     0,     0,     0,    28,
      44,    12,    16,    27,    61,    60,     0,     0,    46,    59,
      62,    65,     0,    63,    78,     0,     0,    69,    58,    83,
       0,     0,    76,     0,    70,    79,     0,    80,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      82,     0,     0,     0,     0,    84,    45,     0,     0,     0,
       0,    74,    75,    71,    72,    73,     0,    85,     0,    87,
       0,    66,    67,    64,     0,     0,    86,     0,    77,    81,
      68
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,     3,     7,    11,    14,    15,    40,    77,    78,
      17,    21,    23,    28,    29,    47,    48,    57,    31,    37,
      38,    51,    71,    98,   109,    39,    62,    63,    74,    75,
      90,    99,   113,   124,   114,   120
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -97
static const yytype_int16 yypact[] =
{
       0,    13,    18,    11,    34,   -97,    69,    66,   -97,    65,
      85,    79,   -97,    67,    85,   -97,    88,    83,    91,   -97,
     -97,    56,    92,    82,    72,   -97,    94,    73,    92,   -97,
      70,   -97,   -97,   -97,    22,   -97,    97,    17,   -97,   -97,
      76,    84,   -97,    99,   -97,    74,    41,    81,    77,    75,
     -97,    93,    86,   106,    87,    41,   -97,    47,   -97,   107,
     -97,   -97,   -97,    19,   109,   -97,   -97,    51,   -97,    41,
     -16,   110,   -97,   -97,   -97,    19,   -97,    90,    95,   -97,
     -97,    45,   -97,   -97,   -97,   -97,    96,   111,    89,   -97,
      -3,   -97,   -97,   -97,   -97,   -97,     1,     8,   115,   -97,
     -97,   -97,   -17,   -97,   -97,     6,     3,   -97,   -97,    98,
      60,   101,   -97,    49,   -97,   -97,    78,   -97,    -3,    46,
     100,   122,   123,   124,   105,     8,     8,     8,     8,     8,
     -97,   112,   103,   126,   104,   -97,   -97,    -7,   102,   108,
       6,   -97,   -97,   -97,   -97,   -97,   128,   -97,   113,   -97,
     129,   -97,   -97,   -97,    42,   114,   -97,   116,   -97,   -97,
     -97
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -97,   -97,   -97,   -97,   -97,   -97,   125,   -97,   -97,   -97,
     -97,   -97,   -97,   -97,   117,   -56,   -43,   118,   -97,   -97,
     119,   -97,   -97,   -97,   -97,   -97,   -97,   -97,    61,   -97,
      30,   -97,     2,   -97,   -96,   -97
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -40
static const yytype_int16 yytable[] =
{
      94,   107,    95,    56,   100,   110,   115,    73,     1,   104,
     112,   104,    56,    81,   150,    82,     4,   -39,     5,    84,
       6,   111,    41,   101,    42,    41,    80,    42,   151,   141,
     142,   143,   144,   145,    96,   102,    97,   116,    72,   103,
     105,   117,   105,   106,    41,   106,    43,    36,    88,    43,
     132,    44,    45,    46,    44,    45,    46,     8,   125,   126,
     133,   134,   127,   128,   129,   125,   126,   158,    43,   127,
     128,   129,     9,    44,    45,    46,    89,   135,    10,    25,
     121,   122,    68,    69,   130,    26,    79,    69,    12,    13,
      16,    18,    20,    22,    24,    30,    27,    32,    33,    34,
      36,    49,    52,    54,    58,    53,    60,    59,    55,    65,
      70,    61,    76,    64,    92,    66,   131,    83,    86,    91,
      93,   108,   123,   136,    87,   137,   138,   139,   119,   140,
     148,   155,   157,   146,   147,   149,    85,   152,   118,    19,
       0,     0,   154,   153,   156,    35,     0,     0,     0,   159,
       0,   160,     0,     0,     0,     0,    50,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    67
};

static const yytype_int16 yycheck[] =
{
       3,    97,     5,    46,     3,    22,     3,    63,     8,     3,
       4,     3,    55,    29,    21,    31,     3,     0,     0,    75,
       9,    38,     3,    22,     5,     3,    69,     5,    35,   125,
     126,   127,   128,   129,    37,    34,    39,    34,    19,    38,
      34,    38,    34,    37,     3,    37,    27,    30,     3,    27,
       4,    32,    33,    34,    32,    33,    34,    23,    16,    17,
      14,    15,    20,    21,    22,    16,    17,    25,    27,    20,
      21,    22,     3,    32,    33,    34,    31,    31,    12,    23,
      20,    21,    35,    36,    35,    29,    35,    36,    23,     4,
      11,    24,     4,    10,     3,    13,     4,    25,     4,    26,
      30,     4,    26,     4,    23,    21,    31,    30,    34,     3,
       3,    18,     3,    27,     3,    28,    38,     7,    28,    23,
      31,     6,    21,    23,    29,     3,     3,     3,    30,    24,
       4,     3,     3,    21,    31,    31,    75,    35,   108,    14,
      -1,    -1,   140,    35,    31,    28,    -1,    -1,    -1,    35,
      -1,    35,    -1,    -1,    -1,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    55
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     8,    41,    42,     3,     0,     9,    43,    23,     3,
      12,    44,    23,     4,    45,    46,    11,    50,    24,    46,
       4,    51,    10,    52,     3,    23,    29,     4,    53,    54,
      13,    58,    25,     4,    26,    54,    30,    59,    60,    65,
      47,     3,     5,    27,    32,    33,    34,    55,    56,     4,
      60,    61,    26,    21,     4,    34,    56,    57,    23,    30,
      31,    18,    66,    67,    27,     3,    28,    57,    35,    36,
       3,    62,    19,    55,    68,    69,     3,    48,    49,    35,
      56,    29,    31,     7,    55,    68,    28,    29,     3,    31,
      70,    23,     3,    31,     3,     5,    37,    39,    63,    71,
       3,    22,    34,    38,     3,    34,    37,    74,     6,    64,
      22,    38,     4,    72,    74,     3,    34,    38,    70,    30,
      75,    20,    21,    21,    73,    16,    17,    20,    21,    22,
      35,    38,     4,    14,    15,    31,    23,     3,     3,     3,
      24,    74,    74,    74,    74,    74,    21,    31,     4,    31,
      21,    35,    35,    35,    72,     3,    31,     3,    25,    35,
      35
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:

/* Line 1455 of yacc.c  */
#line 71 "otp-parser.y"
    { input_bytes=2; }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 73 "otp-parser.y"
    { input_bytes=(yyvsp[(2) - (3)]).yint; }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 78 "otp-parser.y"
    { output_bytes=2; }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 80 "otp-parser.y"
    { output_bytes=(yyvsp[(2) - (3)]).yint; }
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 95 "otp-parser.y"
    { store_table((yyvsp[(1) - (4)]).ystring, (yyvsp[(3) - (4)]).yint); }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 106 "otp-parser.y"
    { add_to_table((yyvsp[(1) - (1)]).yint); }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 108 "otp-parser.y"
    { add_to_table((yyvsp[(3) - (3)]).yint); }
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 118 "otp-parser.y"
    { store_state((yyvsp[(1) - (1)]).ystring); }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 120 "otp-parser.y"
    { store_state((yyvsp[(3) - (3)]).ystring); }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 135 "otp-parser.y"
    { store_alias((yyvsp[(1) - (4)]).ystring, (yyvsp[(3) - (4)]).yleft); }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 140 "otp-parser.y"
    { (yyval).yleft = StringLeft((yyvsp[(1) - (1)]).ystring); }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 142 "otp-parser.y"
    { (yyval).yleft = CompleteLeft((yyvsp[(1) - (6)]).yleft, (yyvsp[(3) - (6)]).yint, (yyvsp[(5) - (6)]).yint); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 144 "otp-parser.y"
    { (yyval).yleft = PlusLeft((yyvsp[(1) - (5)]).yleft, (yyvsp[(3) - (5)]).yint); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 146 "otp-parser.y"
    { (yyval).yleft = CompleteLeft((yyvsp[(1) - (4)]).yleft, (yyvsp[(3) - (4)]).yint, (yyvsp[(3) - (4)]).yint); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 148 "otp-parser.y"
    { (yyval).yleft = (yyvsp[(1) - (1)]).yleft; }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 153 "otp-parser.y"
    { (yyval).yleft = SingleLeft((yyvsp[(1) - (1)]).yint); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 155 "otp-parser.y"
    { (yyval).yleft = DoubleLeft((yyvsp[(1) - (3)]).yint, (yyvsp[(3) - (3)]).yint); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 157 "otp-parser.y"
    { (yyval).yleft = WildCard(); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 159 "otp-parser.y"
    { (yyval).yleft = NotChoiceLeft((yyvsp[(3) - (4)]).ylleft); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 161 "otp-parser.y"
    { (yyval).yleft = ChoiceLeft((yyvsp[(2) - (3)]).ylleft); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 163 "otp-parser.y"
    { (yyval).yleft = lookup_alias((yyvsp[(2) - (3)]).ystring); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 168 "otp-parser.y"
    { (yyval).ylleft = llist1((yyvsp[(1) - (1)]).yleft); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 170 "otp-parser.y"
    { (yyval).ylleft = lappend1((yyvsp[(1) - (3)]).ylleft, (yyvsp[(3) - (3)]).yleft); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 175 "otp-parser.y"
    {
	  for(cur_state=0; cur_state<no_states; cur_state++) {
		  if ((states[cur_state].no_exprs)==0) {
        	     out_int(OTP_LEFT_START, 0);
		  } else {
        	     out_int(OTP_LEFT_RETURN, 0);
                  }
		  out_int(OTP_RIGHT_CHAR, 1);
		  out_int(OTP_STOP, 0);
	  }
	}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 195 "otp-parser.y"
    { states[cur_state].no_exprs++; }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 197 "otp-parser.y"
    { out_left((yyvsp[(3) - (3)]).ylleft); right_offset=0; }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 199 "otp-parser.y"
    { right_offset=OTP_PBACK_OFFSET; }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 201 "otp-parser.y"
    { fill_in_left(); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 211 "otp-parser.y"
    { cur_state = 0; }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 213 "otp-parser.y"
    { cur_state = lookup_state((yyvsp[(2) - (3)]).ystring); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 218 "otp-parser.y"
    { (yyval).ylleft = lappend((yyvsp[(1) - (3)]).ylleft, lappend((yyvsp[(2) - (3)]).ylleft, (yyvsp[(3) - (3)]).ylleft)); }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 220 "otp-parser.y"
    { (yyval).ylleft = lappend((yyvsp[(1) - (2)]).ylleft, (yyvsp[(2) - (2)]).ylleft); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 225 "otp-parser.y"
    { (yyval).ylleft = nil; }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 227 "otp-parser.y"
    { (yyval).ylleft = llist1(BeginningLeft()); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 232 "otp-parser.y"
    { (yyval).ylleft = nil; }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 234 "otp-parser.y"
    { (yyval).ylleft = llist1(EndLeft()); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 239 "otp-parser.y"
    { (yyval).ylleft = llist1((yyvsp[(1) - (1)]).yleft); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 241 "otp-parser.y"
    { (yyval).ylleft = lappend1((yyvsp[(1) - (2)]).ylleft, (yyvsp[(2) - (2)]).yleft); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 251 "otp-parser.y"
    {
	 len=strlen((yyvsp[(1) - (1)]).ystring);
	 for (k=0; k<len; k++) {
            out_right(OTP_RIGHT_NUM, ((yyvsp[(1) - (1)]).ystring)[k]);
         }
	}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 258 "otp-parser.y"
    { out_right(OTP_RIGHT_NUM, (yyvsp[(1) - (1)]).yint); }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 260 "otp-parser.y"
    { out_right(OTP_RIGHT_CHAR, (yyvsp[(2) - (2)]).yint); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 262 "otp-parser.y"
    { out_right(OTP_RIGHT_LCHAR, 0); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 264 "otp-parser.y"
    { out_right(OTP_RIGHT_LCHAR, (yyvsp[(5) - (6)]).yint); }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 266 "otp-parser.y"
    {
	 out_right(OTP_RIGHT_SOME, 0); 
	 out_int(0,0);
	}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 271 "otp-parser.y"
    {
	 out_right(OTP_RIGHT_SOME, (yyvsp[(5) - (6)]).yint);
	 out_int(0, 0);
	}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 276 "otp-parser.y"
    {
	 out_right(OTP_RIGHT_SOME, 0);
	 out_int(0, (yyvsp[(5) - (6)]).yint);
	}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 281 "otp-parser.y"
    {
	 out_right(OTP_RIGHT_SOME, (yyvsp[(5) - (8)]).yint);
	 out_int(0, (yyvsp[(7) - (8)]).yint);
	}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 286 "otp-parser.y"
    { out_right(OTP_RIGHT_OUTPUT, 0); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 292 "otp-parser.y"
    { out_int(OTP_ADD, 0); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 294 "otp-parser.y"
    { out_int(OTP_SUB, 0); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 296 "otp-parser.y"
    { out_int(OTP_MULT, 0); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 298 "otp-parser.y"
    { out_int(OTP_DIV, 0); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 300 "otp-parser.y"
    { out_int(OTP_MOD, 0); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 302 "otp-parser.y"
    { out_int(OTP_PUSH_NUM, lookup_table((yyvsp[(1) - (1)]).ystring)); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 304 "otp-parser.y"
    { out_int(OTP_LOOKUP, 0); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 309 "otp-parser.y"
    { out_int(OTP_PUSH_NUM, (yyvsp[(1) - (1)]).yint); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 311 "otp-parser.y"
    { out_int(OTP_PUSH_CHAR, (yyvsp[(2) - (2)]).yint); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 313 "otp-parser.y"
    { out_int(OTP_PUSH_LCHAR, 0); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 315 "otp-parser.y"
    { out_int(OTP_PUSH_LCHAR, (yyvsp[(5) - (6)]).yint); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 322 "otp-parser.y"
    { out_int(OTP_STATE_CHANGE, 0); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 324 "otp-parser.y"
    { out_int(OTP_STATE_CHANGE, lookup_state((yyvsp[(2) - (3)]).ystring)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 326 "otp-parser.y"
    { out_int(OTP_STATE_PUSH, lookup_state((yyvsp[(3) - (4)]).ystring)); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 328 "otp-parser.y"
    { out_int(OTP_STATE_POP, 0); }
    break;



/* Line 1455 of yacc.c  */
#line 1974 "otp-parser.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 330 "otp-parser.y"


