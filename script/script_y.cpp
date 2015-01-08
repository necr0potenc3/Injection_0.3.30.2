
/*  A Bison parser, made from script.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define NUM 258
#define VAR 259
#define SUB 260
#define END_SUB 261
#define IF  262
#define THEN    263
#define ELSE    264
#define END_IF  265
#define GOTO    266
#define RETURN  267
#define DIM 268
#define REPEAT  269
#define UNTIL   270
#define WHILE   271
#define WEND    272
#define FOR 273
#define TO  274
#define STEP    275
#define NEXT    276
#define SAFECALL    277
#define FATALERROR  278
#define RETURNERROR 279
#define IDENTIFIER  280
#define OR  281
#define AND 282
#define NOT 283
#define NEG 284


#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "myvar.h"
#include "myparser.h"

#define YYSTYPE TVariable
#define YYPARSE_PARAM This
#define YYLEX_PARAM This
#define yyerror(a) _yyerror(This,a)

// начальное значение размера стека
// для экономии стека при рекурсии
#define YYINITDEPTH 15

#ifdef THIS
#undef THIS
#endif

#define THIS ((TParser*)This)

void _yyerror(void *, char*);
//int yylex (YYSTYPE *lval, void *This);
int yyparse (void *);
void SkipToEndif (void *This);
void SkipToElseOrEndif (void *This);
void SkipToWend (void *This);
void SkipToNext (void *This);

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define YYFINAL     142
#define YYFLAG      -32768
#define YYNTBASE    44

#define YYTRANSLATE(x) ((unsigned)(x) <= 284 ? yytranslate[x] : 65)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    37,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    39,
    40,    34,    33,    43,    32,     2,    35,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    38,     2,    29,
    31,    30,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    41,     2,    42,     2,     2,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    36
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     6,     9,    13,    16,    19,    22,    25,
    29,    35,    37,    40,    42,    44,    47,    51,    53,    55,
    58,    60,    62,    63,    70,    72,    75,    78,    82,    89,
    92,    96,   101,   102,   104,   105,   107,   108,   110,   111,
   113,   115,   119,   121,   125,   127,   131,   133,   137,   142,
   149,   153,   154,   160,   161,   168,   170,   172,   177,   179,
   181,   183,   187,   191,   195,   199,   203,   207,   210,   213,
   217,   222,   227,   232,   236,   241
};

static const short yyrhs[] = {    -1,
    44,    45,     0,    37,     0,    46,    37,     0,    46,    47,
    37,     0,    47,    37,     0,    25,    38,     0,     4,    57,
     0,    13,    58,     0,    49,    19,    64,     0,    49,    19,
    64,    20,    64,     0,    21,     0,    16,    64,     0,    17,
     0,    14,     0,    15,    64,     0,     7,    64,     8,     0,
     9,     0,    10,     0,    11,    25,     0,    62,     0,    60,
     0,     0,     5,    25,    48,    39,    53,    40,     0,     6,
     0,    12,    51,     0,    24,    52,     0,    25,    31,    64,
     0,    25,    41,    64,    42,    31,    64,     0,    18,    50,
     0,    25,    31,    64,     0,     4,    25,    31,    64,     0,
     0,    64,     0,     0,    64,     0,     0,    55,     0,     0,
    56,     0,    25,     0,    55,    43,    25,     0,    64,     0,
    56,    43,    64,     0,    25,     0,    57,    43,    25,     0,
    59,     0,    57,    43,    59,     0,    25,    41,    64,    42,
     0,    58,    43,    25,    41,    64,    42,     0,    25,    31,
    64,     0,     0,    25,    39,    61,    54,    40,     0,     0,
    22,    25,    39,    63,    54,    40,     0,     3,     0,    25,
     0,    25,    41,    64,    42,     0,    60,     0,    62,     0,
    23,     0,    64,    33,    64,     0,    64,    32,    64,     0,
    64,    34,    64,     0,    64,    35,    64,     0,    64,    27,
    64,     0,    64,    26,    64,     0,    32,    64,     0,    28,
    64,     0,    39,    64,    40,     0,    64,    31,    31,    64,
     0,    64,    29,    31,    64,     0,    64,    30,    31,    64,
     0,    64,    29,    64,     0,    64,    29,    30,    64,     0,
    64,    30,    64,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   126,   127,   131,   132,   133,   134,   138,   144,   149,   155,
   163,   175,   188,   205,   211,   214,   225,   234,   239,   242,
   245,   246,   249,   252,   261,   267,   271,   276,   281,   289,
   294,   299,   311,   312,   319,   320,   329,   331,   335,   337,
   341,   347,   355,   361,   370,   375,   380,   381,   385,   390,
   398,   409,   413,   423,   425,   444,   445,   449,   453,   454,
   455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
   465,   466,   467,   468,   469,   470
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","NUM","VAR",
"SUB","END_SUB","IF","THEN","ELSE","END_IF","GOTO","RETURN","DIM","REPEAT","UNTIL",
"WHILE","WEND","FOR","TO","STEP","NEXT","SAFECALL","FATALERROR","RETURNERROR",
"IDENTIFIER","OR","AND","NOT","'<'","'>'","'='","'-'","'+'","'*'","'/'","NEG",
"'\\n'","':'","'('","')'","'['","']'","','","input","line","label","commands",
"@1","for_closure","for_exp","ret_stmt","err_stmt","sub_parameters","call_parameters",
"parameter_list","call_parameter_list","list_of_variables","list_of_arrays",
"assignment_identifier","function_call","@2","safe_function_call","@3","exp", NULL
};
#endif

static const short yyr1[] = {     0,
    44,    44,    45,    45,    45,    45,    46,    47,    47,    47,
    47,    47,    47,    47,    47,    47,    47,    47,    47,    47,
    47,    47,    48,    47,    47,    47,    47,    47,    47,    49,
    50,    50,    51,    51,    52,    52,    53,    53,    54,    54,
    55,    55,    56,    56,    57,    57,    57,    57,    58,    58,
    59,    61,    60,    63,    62,    64,    64,    64,    64,    64,
    64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
    64,    64,    64,    64,    64,    64
};

static const short yyr2[] = {     0,
     0,     2,     1,     2,     3,     2,     2,     2,     2,     3,
     5,     1,     2,     1,     1,     2,     3,     1,     1,     2,
     1,     1,     0,     6,     1,     2,     2,     3,     6,     2,
     3,     4,     0,     1,     0,     1,     0,     1,     0,     1,
     1,     3,     1,     3,     1,     3,     1,     3,     4,     6,
     3,     0,     5,     0,     6,     1,     1,     4,     1,     1,
     1,     3,     3,     3,     3,     3,     3,     2,     2,     3,
     4,     4,     4,     3,     4,     3
};

static const short yydefact[] = {     1,
     0,     0,     0,    25,     0,    18,    19,     0,    33,     0,
    15,     0,     0,    14,     0,    12,     0,    35,     0,     3,
     2,     0,     0,     0,    22,    21,    45,     8,    47,    23,
    56,    61,    57,     0,     0,     0,    59,    60,     0,    20,
    26,    34,     0,     9,    16,    13,     0,     0,    30,     0,
    27,    36,     0,     7,    52,     0,     0,     4,     0,     6,
     0,     0,     0,     0,     0,    69,    68,     0,    17,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    54,    28,    39,     0,     5,    10,    51,    46,
    48,    37,     0,    70,    67,    66,     0,     0,    74,     0,
    76,     0,    63,    62,    64,    65,     0,     0,     0,    31,
    39,     0,    40,    43,     0,     0,    41,     0,    38,    58,
    75,    72,    73,    71,    49,     0,    32,     0,    53,     0,
     0,    11,    24,     0,     0,    55,    44,    29,    42,    50,
     0,     0
};

static const short yydefgoto[] = {     1,
    21,    22,    23,    64,    24,    49,    41,    51,   118,   112,
   119,   113,    28,    44,    29,    37,    85,    38,   111,   114
};

static const short yypact[] = {-32768,
   123,   -19,   -10,-32768,    77,-32768,-32768,    -4,    77,    10,
-32768,    77,    77,-32768,     7,-32768,    20,    77,   -29,-32768,
-32768,   152,     3,    -1,-32768,-32768,    18,    21,-32768,-32768,
-32768,-32768,   -36,    77,    77,    77,-32768,-32768,    -7,-32768,
-32768,   234,    14,    26,   234,   234,    28,    27,-32768,    22,
-32768,   234,    77,-32768,-32768,    77,    13,-32768,    38,-32768,
    77,    77,    34,    24,    77,   120,-32768,   222,-32768,    77,
    77,    11,    59,    45,    77,    77,    77,    77,    77,    53,
    48,    77,-32768,   234,    77,   153,-32768,   212,   234,    18,
-32768,    58,   167,-32768,   243,   120,    77,    77,    80,    77,
    80,    77,    12,    12,-32768,-32768,   181,    44,    77,   234,
    77,    46,    51,   234,    57,    77,-32768,    49,    60,-32768,
    80,    80,    80,    80,-32768,    77,   234,    56,-32768,    77,
    77,   234,-32768,    76,   195,-32768,   234,   234,-32768,-32768,
   106,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    85,-32768,-32768,-32768,-32768,-32768,-32768,    -3,
-32768,-32768,-32768,-32768,    47,    15,-32768,    16,-32768,    -5
};


#define YYLAST      278


static const short yytable[] = {    39,
    69,    53,    55,    42,    65,    27,    45,    46,    54,    55,
    47,    56,    52,    31,    30,    25,    26,    61,    70,    71,
    40,    72,    73,    74,    75,    76,    77,    78,    66,    67,
    68,    48,    17,    32,    43,    33,    25,    26,    34,    60,
    97,    98,    35,    53,    50,    77,    78,    84,    62,    36,
    86,    55,    81,    56,    79,    88,    89,    82,    90,    93,
    83,    31,    92,    63,    95,    96,    99,   101,    80,   103,
   104,   105,   106,   107,    87,   102,   110,   108,   109,    31,
    17,    32,   117,    33,   126,   129,    34,   131,   133,   100,
    35,   121,   122,   130,   123,   136,   124,    36,    17,    32,
   139,    33,   134,   127,    34,   142,    59,   128,    35,    91,
   132,    75,    76,    77,    78,    36,     0,     0,     0,     0,
   135,     0,   141,     0,   137,   138,     2,     3,     4,     5,
     0,     6,     7,     8,     9,    10,    11,    12,    13,    14,
    15,     0,     0,    16,    17,     0,    18,    19,    72,    73,
    74,    75,    76,    77,    78,     2,     3,     4,     5,    20,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
     0,     0,    16,    17,     0,    18,    57,     0,    70,    71,
     0,    72,    73,    74,    75,    76,    77,    78,    58,     0,
     0,     0,    70,    71,   115,    72,    73,    74,    75,    76,
    77,    78,     0,     0,     0,     0,    70,    71,   120,    72,
    73,    74,    75,    76,    77,    78,     0,     0,     0,     0,
    70,    71,   125,    72,    73,    74,    75,    76,    77,    78,
     0,   116,     0,     0,     0,     0,   140,    70,    71,     0,
    72,    73,    74,    75,    76,    77,    78,    70,    71,     0,
    72,    73,    74,    75,    76,    77,    78,     0,     0,    70,
    71,    94,    72,    73,    74,    75,    76,    77,    78,    71,
     0,    72,    73,    74,    75,    76,    77,    78
};

static const short yycheck[] = {     5,
     8,    31,    39,     9,    41,    25,    12,    13,    38,    39,
     4,    41,    18,     3,    25,     1,     1,    19,    26,    27,
    25,    29,    30,    31,    32,    33,    34,    35,    34,    35,
    36,    25,    22,    23,    25,    25,    22,    22,    28,    37,
    30,    31,    32,    31,    25,    34,    35,    53,    31,    39,
    56,    39,    25,    41,    41,    61,    62,    31,    25,    65,
    39,     3,    39,    43,    70,    71,    72,    73,    43,    75,
    76,    77,    78,    79,    37,    31,    82,    25,    31,     3,
    22,    23,    25,    25,    41,    40,    28,    31,    40,    31,
    32,    97,    98,    43,   100,    40,   102,    39,    22,    23,
    25,    25,    43,   109,    28,     0,    22,   111,    32,    63,
   116,    32,    33,    34,    35,    39,    -1,    -1,    -1,    -1,
   126,    -1,     0,    -1,   130,   131,     4,     5,     6,     7,
    -1,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    18,    -1,    -1,    21,    22,    -1,    24,    25,    29,    30,
    31,    32,    33,    34,    35,     4,     5,     6,     7,    37,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
    -1,    -1,    21,    22,    -1,    24,    25,    -1,    26,    27,
    -1,    29,    30,    31,    32,    33,    34,    35,    37,    -1,
    -1,    -1,    26,    27,    42,    29,    30,    31,    32,    33,
    34,    35,    -1,    -1,    -1,    -1,    26,    27,    42,    29,
    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
    26,    27,    42,    29,    30,    31,    32,    33,    34,    35,
    -1,    20,    -1,    -1,    -1,    -1,    42,    26,    27,    -1,
    29,    30,    31,    32,    33,    34,    35,    26,    27,    -1,
    29,    30,    31,    32,    33,    34,    35,    -1,    -1,    26,
    27,    40,    29,    30,    31,    32,    33,    34,    35,    27,
    -1,    29,    30,    31,    32,    33,    34,    35
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */


/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         -2
#define YYEOF           0
#define YYACCEPT        return(0)
#define YYABORT         return(1)
#define YYERROR         goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL          goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do                                                              \
  if (yychar == YYEMPTY && yylen == 1)                          \
    { yychar = (token), yylval = (value);                       \
      yychar1 = YYTRANSLATE (yychar);                           \
      YYPOPSTACK;                                               \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    { yyerror ("syntax error: cannot back up"); YYERROR; }      \
while (0)

#define YYTERROR        1
#define YYERRCODE       256

#ifndef YYPURE
#define YYLEX           yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX           yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX           yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX           yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX           yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int     yychar;                 /*  the lookahead symbol                */
YYSTYPE yylval;                 /*  the semantic value of the           */
                                /*  lookahead symbol                    */

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;                 /*  location data for the lookahead     */
                                /*  symbol                              */
#endif

int yynerrs;                    /*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;                    /*  nonzero means print parse trace     */
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks       */

#ifndef YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1                /* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)      __builtin_memcpy(TO,FROM,COUNT)
#else                           /* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;      /*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;              /*  lookahead token as an internal (translated) token number */

  short yyssa[YYINITDEPTH];     /*  the state stack                     */
  YYSTYPE yyvsa[YYINITDEPTH];   /*  the semantic value stack            */

  short *yyss = yyssa;          /*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;        /*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];   /*  the location stack                  */
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;                /*  the variable used to return         */
                                /*  semantic values from the action     */
                                /*  routines                            */

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;             /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
         the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
         but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
                 &yyss1, size * sizeof (*yyssp),
                 &yyvs1, size * sizeof (*yyvsp),
                 &yyls1, size * sizeof (*yylsp),
                 &yystacksize);
#else
      yyoverflow("parser stack overflow",
                 &yyss1, size * sizeof (*yyssp),
                 &yyvs1, size * sizeof (*yyvsp),
                 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
        {
          yyerror("parser stack overflow");
          return 2;
        }
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
        yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      memset(yyvs,0,yystacksize * sizeof (*yyvsp));
      for (int tt=0; tt<size ; tt++)
        yyvs[tt]=yyvs1[tt];

// -SASHA-      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
        fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
        YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
        fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)              /* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;           /* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
        fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
        {
          fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
          /* Give the individual parser a way to print the precise meaning
             of a token, for further debugging info.  */
#ifdef YYPRINT
          YYPRINT (stderr, yychar, yylval);
#endif
          fprintf (stderr, ")\n");
        }
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
               yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
        fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 7:
{ THIS->AddLabel(yyvsp[-1]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 10:
{ if(!THIS->IsPreprocessing)
                                  {
                                      THIS->SetStepForLastFor(1.0);
                                      THIS->SetEndForLastFor(yyvsp[0]); 
                                      THIS->SetForPos();
                                  }
                                  THIS->CycleDepth++;
                                ;
    break;}
case 11:
{ if(!THIS->IsPreprocessing)
                                  {
                                      THIS->SetStepForLastFor(yyvsp[0]);
                                      THIS->SetEndForLastFor(yyvsp[-2]);
                                      THIS->SetForPos();
                                  }
                                  THIS->CycleDepth++;
                                ;
    break;}
case 12:
{ if(!THIS->IsPreprocessing)
                                  {
                                    if(!THIS->LastForIsEqual())  // если не выход из цикла
                                      THIS->GoToLastFor();       // то перейти на строчку после FOR
                                    else
                                    {
                                      THIS->CycleDepth--;        // иначе убрать этот FOR
                                      THIS->PopFor();
                                    }
                                  } else
                                      THIS->CycleDepth--;
                                ;
    break;}
case 13:
{ if(THIS->IsPreprocessing)
                                  {
                                    THIS->CycleDepth++;
                                  }
                                  else if(!yyvsp[0].IsTrue())   // если условие ложно,
                                  {
                                    THIS->CycleDepth++;     // увеличить счетчик вложенности циклов (для меток)
                                    SkipToWend(This);       // проскипать до WEND
                                    THIS->CycleDepth--;
                                  }
                                  else
                                  {
                                    THIS->PushWhile();      // ну а если истинно - запомнить позицию перед WHILE
                                    THIS->CycleDepth++;
                                  }
                                ;
    break;}
case 14:
{ if(!THIS->IsPreprocessing) // грубо говоря: перейти на позицию 
                                    THIS->PopWhile();        // перед соответствующим WHILE - а там разберемся
                                  THIS->CycleDepth--;
                                ;
    break;}
case 15:
{ THIS->PushRepeat(); THIS->CycleDepth++;  ;
    break;}
case 16:
{ if(!yyvsp[0].IsTrue()&&!THIS->IsPreprocessing)
                                  {
                                    THIS->GoToLastRepeat(); // если условие ложно то перейти на REPEAT
                                  }
                                  else
                                  {
                                    THIS->PopRepeat();      // если истинно - забыть о последнем REPEATе 
                                    THIS->CycleDepth--;
                                  }
                                ;
    break;}
case 17:
{ if(!THIS->IsPreprocessing)
                                    if(!(yyvsp[-1].IsTrue()))
                                      SkipToElseOrEndif(This);
                                   // если условие ложно - скипать до ELSE, а если ELSE нету, то до ENDIF
                                   // ну а если истинно - ничего не делать
                                ;
    break;}
case 18:
{ if(!THIS->IsPreprocessing)
                                    SkipToEndif(This);  // пропустить "ложную" часть
                                ;
    break;}
case 20:
{ THIS->GoTo(yyvsp[0]); ;
    break;}
case 23:
{ THIS->BeginCountingParameters(yyvsp[0]);;
    break;}
case 24:
{ THIS->AddSub(yyvsp[-4]);  // этот код выполняется после
                                  if(THIS->Error)    // завершения разбора sub_parameters
                                    YYABORT;
                                  THIS->BeginSub(yyvsp[-4]);// сказать парсеру, что вошли в процедуру
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 25:
{ THIS->EndSub();    // вышли из процедуры
                                  if(THIS->FinishedSub)
                                    YYACCEPT;        // завершить yyparse, если он был вызван из TParser::Execute
                                ;
    break;}
case 26:
{ if(!THIS->IsPreprocessing)
                                    YYACCEPT;  // завершить yyparse
                                ;
    break;}
case 27:
{ if(!THIS->IsPreprocessing)
                                    YYACCEPT;  // завершить yyparse
                                ;
    break;}
case 28:
{ THIS->SetVar (yyvsp[-2], yyvsp[0]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 29:
{ THIS->SetArray (yyvsp[-5], yyvsp[-3], yyvsp[0]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 30:
{ if(!THIS->IsPreprocessing)
                                    THIS->PushFor();    ;
    break;}
case 31:
{ THIS->SetVar (yyvsp[-2], yyvsp[0]);
                                  if(THIS->Error)
                                    YYABORT;
                                  THIS->SetLastForVar(yyvsp[-2]);
                                ;
    break;}
case 32:
{ THIS->AddVar (yyvsp[-2]);
                                  if(THIS->Error)
                                    YYABORT;
                                  THIS->SetVar (yyvsp[-2], yyvsp[0]);
                                  if(THIS->Error)
                                    YYABORT;
                                  THIS->SetLastForVar(yyvsp[-2]);
                                ;
    break;}
case 34:
{ yyval = THIS->Result = yyvsp[0]; 
                                  if(yyvsp[0].ErrorCode()&&!THIS->IsPreprocessing)
                                    THIS->SetRuntimeError(yyvsp[0]);
                                ;
    break;}
case 36:
{
                                    if(yyvsp[0].Type==TVariable::T_String)
                                        THIS->Result = ::Error(yyvsp[0].Data.AsString.c_str());
                                    else
                                        THIS->Result = ::Error(yyvsp[0].Data.AsNumber);
                                ;
    break;}
case 41:
{ THIS->AddParameter(yyvsp[0]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 42:
{ THIS->AddParameter(yyvsp[0]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 43:
{ THIS->StoreParameter(yyvsp[0]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 44:
{ THIS->StoreParameter(yyvsp[0]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 45:
{ THIS->AddVar (yyvsp[0]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 46:
{ THIS->AddVar (yyvsp[0]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 49:
{ THIS->AddArray (yyvsp[-3], yyvsp[-1]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 50:
{ THIS->AddArray (yyvsp[-3], yyvsp[-1]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 51:
{ THIS->AddVar (yyvsp[-2]);
                                  if(THIS->Error)
                                    YYABORT;
                                  THIS->SetVar (yyvsp[-2], yyvsp[0]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 52:
{ THIS->BeginCollectingParameters();;
    break;}
case 53:
{ yyval = THIS->CallSub(yyvsp[-4]); // при встрече ')' идет вызов процедуры
                                  THIS->EndCollectingParameters();
                                  if(THIS->Error)
                                    YYABORT;
                                  if(yyval.ErrorCode())
                                    THIS->SetRuntimeError(yyval);
                                ;
    break;}
case 54:
{ THIS->BeginCollectingParameters();;
    break;}
case 55:
{ TVariable tmp = THIS->CallSub(yyvsp[-4]);
                                  THIS->EndCollectingParameters();
                                  if(THIS->Error)
                                    YYABORT;
                                  if(tmp.ErrorCode())   // тут запоминается значение
                                  {                     // ошибки (если была)
                                    THIS->FatalError=tmp;
                                    THIS->FatalError.Type=tmp.Data.AsString.Length()?TVariable::T_String :
                                        TVariable::T_Number;
                                    yyval=THIS->FatalError;
                                  } else
                                  {
                                    THIS->FatalError=0.0;
                                    yyval=tmp;
                                  }
                                ;
    break;}
case 56:
{ yyval = yyvsp[0];         ;
    break;}
case 57:
{ yyval = THIS->GetVar(yyvsp[0]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 58:
{ yyval = THIS->GetArray(yyvsp[-3],yyvsp[-1]);
                                  if(THIS->Error)
                                    YYABORT;
                                ;
    break;}
case 59:
{ yyval = yyvsp[0];         ;
    break;}
case 60:
{ yyval = yyvsp[0];         ;
    break;}
case 61:
{ yyval = THIS->FatalError;    ;
    break;}
case 62:
{ yyval = yyvsp[-2] + yyvsp[0];    ;
    break;}
case 63:
{ yyval = yyvsp[-2] - yyvsp[0];    ;
    break;}
case 64:
{ yyval = yyvsp[-2] * yyvsp[0];    ;
    break;}
case 65:
{ yyval = yyvsp[-2] / yyvsp[0];    ;
    break;}
case 66:
{ yyval = yyvsp[-2].IsTrue() && yyvsp[0].IsTrue(); ;
    break;}
case 67:
{ yyval = yyvsp[-2].IsTrue() || yyvsp[0].IsTrue(); ;
    break;}
case 68:
{ yyval = -yyvsp[0];        ;
    break;}
case 69:
{ yyval = !yyvsp[0].IsTrue(); ;
    break;}
case 70:
{ yyval = yyvsp[-1];         ;
    break;}
case 71:
{ yyval = yyvsp[-3] == yyvsp[0];   ;
    break;}
case 72:
{ yyval = (yyvsp[-3] < yyvsp[0]) || (yyvsp[-3] == yyvsp[0]);   ;
    break;}
case 73:
{ yyval = !(yyvsp[-3] < yyvsp[0]); ;
    break;}
case 74:
{ yyval = yyvsp[-2] < yyvsp[0];    ;
    break;}
case 75:
{ yyval = !(yyvsp[-3] == yyvsp[0]);;
    break;}
case 76:
{ yyval = !(yyvsp[-2] < yyvsp[0]) && !(yyvsp[-2] == yyvsp[0]); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */


  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
        fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
        {
          int size = 0;
          char *msg;
          int x, count;

          count = 0;
          /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
          for (x = (yyn < 0 ? -yyn : 0);
               x < (sizeof(yytname) / sizeof(char *)); x++)
            if (yycheck[x + yyn] == x)
              size += strlen(yytname[x]) + 15, count++;
          msg = (char *) malloc(size + 15);
          if (msg != 0)
            {
              strcpy(msg, "parse error");

              if (count < 5)
                {
                  count = 0;
                  for (x = (yyn < 0 ? -yyn : 0);
                       x < (sizeof(yytname) / sizeof(char *)); x++)
                    if (yycheck[x + yyn] == x)
                      {
                        strcat(msg, count == 0 ? ", expecting `" : " or `");
                        strcat(msg, yytname[x]);
                        strcat(msg, "'");
                        count++;
                      }
                }
              yyerror(msg);
              free(msg);
            }
          else
            yyerror ("parse error; also virtual memory exceeded");
        }
      else
#endif /* YYERROR_VERBOSE */
        yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
        YYABORT;

#if YYDEBUG != 0
      if (yydebug)
        fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;              /* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
        fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
        goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}



#include <stdio.h>

void _yyerror (void *This, char *s)  /* Called by yyparse on error */
{
    THIS->SetParserError(s);
}

