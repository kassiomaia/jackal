/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "jackal_parser.y"

#include <jackal.h>

extern int yylineno;
extern char *yytext;
extern int yylex(void);
int yyerror(char*);

jkl_program_t *program;

#define USE_EVAL
#undef USE_EVAL


#line 86 "jackal_parser.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "jackal_parser.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ID = 3,                         /* ID  */
  YYSYMBOL_CINT = 4,                       /* CINT  */
  YYSYMBOL_CFLOAT = 5,                     /* CFLOAT  */
  YYSYMBOL_CSTRING = 6,                    /* CSTRING  */
  YYSYMBOL_LET = 7,                        /* "let"  */
  YYSYMBOL_ASSIGN = 8,                     /* ":="  */
  YYSYMBOL_LBRACE = 9,                     /* "{"  */
  YYSYMBOL_RBRACE = 10,                    /* "}"  */
  YYSYMBOL_EQL = 11,                       /* "=="  */
  YYSYMBOL_NEQ = 12,                       /* "!="  */
  YYSYMBOL_GT = 13,                        /* ">"  */
  YYSYMBOL_LT = 14,                        /* "<"  */
  YYSYMBOL_GTE = 15,                       /* ">="  */
  YYSYMBOL_LTE = 16,                       /* "<="  */
  YYSYMBOL_PLUS = 17,                      /* "+"  */
  YYSYMBOL_MINUS = 18,                     /* "-"  */
  YYSYMBOL_MUL = 19,                       /* "*"  */
  YYSYMBOL_DIV = 20,                       /* "/"  */
  YYSYMBOL_MOD = 21,                       /* "%"  */
  YYSYMBOL_AND = 22,                       /* "&&"  */
  YYSYMBOL_OR = 23,                        /* "||"  */
  YYSYMBOL_NOT = 24,                       /* "!"  */
  YYSYMBOL_LOOP = 25,                      /* "loop"  */
  YYSYMBOL_RAISE = 26,                     /* "raise"  */
  YYSYMBOL_IF = 27,                        /* "if"  */
  YYSYMBOL_ELIF = 28,                      /* "elif"  */
  YYSYMBOL_ELSE = 29,                      /* "else"  */
  YYSYMBOL_FUNC = 30,                      /* "func"  */
  YYSYMBOL_RETURN = 31,                    /* "return"  */
  YYSYMBOL_LPAREN = 32,                    /* "("  */
  YYSYMBOL_RPAREN = 33,                    /* ")"  */
  YYSYMBOL_COMMA = 34,                     /* ","  */
  YYSYMBOL_YYACCEPT = 35,                  /* $accept  */
  YYSYMBOL_program = 36,                   /* program  */
  YYSYMBOL_statements = 37,                /* statements  */
  YYSYMBOL_statement = 38,                 /* statement  */
  YYSYMBOL_expr = 39,                      /* expr  */
  YYSYMBOL_term = 40,                      /* term  */
  YYSYMBOL_ident = 41,                     /* ident  */
  YYSYMBOL_loop = 42,                      /* loop  */
  YYSYMBOL_43_1 = 43,                      /* $@1  */
  YYSYMBOL_if_stm = 44,                    /* if_stm  */
  YYSYMBOL_if_then = 45,                   /* if_then  */
  YYSYMBOL_46_2 = 46,                      /* @2  */
  YYSYMBOL_else_opt = 47,                  /* else_opt  */
  YYSYMBOL_48_3 = 48,                      /* $@3  */
  YYSYMBOL_49_4 = 49,                      /* @4  */
  YYSYMBOL_50_5 = 50,                      /* @5  */
  YYSYMBOL_block_stmts = 51,               /* block_stmts  */
  YYSYMBOL_call = 52,                      /* call  */
  YYSYMBOL_func = 53,                      /* func  */
  YYSYMBOL_54_6 = 54,                      /* $@6  */
  YYSYMBOL_func_params = 55,               /* func_params  */
  YYSYMBOL_56_7 = 56,                      /* $@7  */
  YYSYMBOL_params = 57,                    /* params  */
  YYSYMBOL_param = 58                      /* param  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   229

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  35
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  24
/* YYNRULES -- Number of rules.  */
#define YYNRULES  57
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  99

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   289


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    81,    81,    87,    93,    94,    95,    98,   117,   118,
     129,   140,   141,   152,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   172,
     173,   181,   189,   199,   209,   209,   232,   235,   235,   252,
     254,   254,   263,   276,   263,   285,   286,   287,   290,   301,
     312,   312,   329,   329,   337,   338,   343,   350
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ID", "CINT", "CFLOAT",
  "CSTRING", "\"let\"", "\":=\"", "\"{\"", "\"}\"", "\"==\"", "\"!=\"",
  "\">\"", "\"<\"", "\">=\"", "\"<=\"", "\"+\"", "\"-\"", "\"*\"", "\"/\"",
  "\"%\"", "\"&&\"", "\"||\"", "\"!\"", "\"loop\"", "\"raise\"", "\"if\"",
  "\"elif\"", "\"else\"", "\"func\"", "\"return\"", "\"(\"", "\")\"",
  "\",\"", "$accept", "program", "statements", "statement", "expr", "term",
  "ident", "loop", "$@1", "if_stm", "if_then", "@2", "else_opt", "$@3",
  "@4", "@5", "block_stmts", "call", "func", "$@6", "func_params", "$@7",
  "params", "param", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-73)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -73,    21,   -73,     4,     2,     5,    13,    50,     2,    50,
     141,   -73,   -73,   -73,   -11,   -73,   -73,   -73,   -73,   -73,
      14,   -73,   -73,   -73,   -73,   -73,    50,   185,   -73,   -73,
      -9,   185,   -73,    50,   -73,   -73,    50,   141,   162,    50,
      50,    50,    50,    50,    50,    50,    50,    50,    50,    50,
      50,    50,    17,   -73,    20,   185,    22,   185,   -73,    67,
     -73,   173,   173,   208,   208,   208,   208,    -8,    -8,   -73,
     -73,   -73,    48,   198,   141,     2,   -73,    23,   141,   -73,
     -73,    92,   -73,    16,   -73,   141,   141,   100,   -73,   -73,
       2,   125,   133,   -73,   -73,   -73,   -73,   -11,   -73
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       3,     6,     8,    11,    39,    10,    13,    33,    48,    49,
       0,    34,     9,    30,    32,    31,     0,    37,    28,    29,
       0,    12,     5,     0,    40,    36,     0,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    52,     0,    42,     0,     7,    47,     0,
      27,    19,    20,    21,    23,    22,    24,    14,    15,    16,
      17,    18,    25,    26,    45,    54,    50,     0,    45,    35,
      46,     0,    57,     0,    56,    45,    45,     0,    38,    53,
       0,     0,     0,    41,    55,    51,    43,    39,    44
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -73,   -73,   -73,    -1,    -6,   -73,    -2,   -73,   -73,   -73,
     -73,   -73,   -72,   -73,   -73,   -73,   -70,   -73,   -73,   -73,
     -73,   -73,   -73,   -33
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,    10,    58,    27,    28,    29,    12,    37,    13,
      14,    52,    35,    56,    77,    97,    59,    15,    16,    85,
      54,    75,    83,    84
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      11,    19,    20,    31,    81,    17,    30,    17,    87,    32,
      18,    47,    48,    49,    21,    91,    92,    33,    34,    22,
      38,     2,    36,    53,     3,    98,    74,    55,     4,    76,
      57,    78,    86,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,     5,     6,     7,    89,
      90,     8,     9,    17,    23,    24,    25,    94,    80,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
       3,     0,     0,    82,     4,     0,     0,    79,     0,     0,
      80,     0,    26,     0,     0,     0,    80,     0,    82,     0,
      80,    80,     5,     6,     7,     3,     0,     8,     9,     4,
       0,     0,    88,     3,     0,     0,     0,     4,     0,     0,
      93,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     0,     8,     9,     0,     5,     6,     7,     3,     0,
       8,     9,     4,     0,     0,    95,     3,     0,     0,     0,
       4,     0,     0,    96,     3,     0,     0,     0,     4,     0,
       5,     6,     7,     0,     0,     8,     9,     0,     5,     6,
       7,     0,     0,     8,     9,     0,     5,     6,     7,     0,
       0,     8,     9,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    60,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49
};

static const yytype_int8 yycheck[] =
{
       1,     3,     4,     9,    74,     3,     8,     3,    78,    10,
       6,    19,    20,    21,     9,    85,    86,    28,    29,     6,
      26,     0,     8,    32,     3,    97,     9,    33,     7,     9,
      36,     9,     9,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    25,    26,    27,    33,
      34,    30,    31,     3,     4,     5,     6,    90,    59,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
       3,    -1,    -1,    75,     7,    -1,    -1,    10,    -1,    -1,
      81,    -1,    32,    -1,    -1,    -1,    87,    -1,    90,    -1,
      91,    92,    25,    26,    27,     3,    -1,    30,    31,     7,
      -1,    -1,    10,     3,    -1,    -1,    -1,     7,    -1,    -1,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    25,    26,    27,
      -1,    -1,    30,    31,    -1,    25,    26,    27,     3,    -1,
      30,    31,     7,    -1,    -1,    10,     3,    -1,    -1,    -1,
       7,    -1,    -1,    10,     3,    -1,    -1,    -1,     7,    -1,
      25,    26,    27,    -1,    -1,    30,    31,    -1,    25,    26,
      27,    -1,    -1,    30,    31,    -1,    25,    26,    27,    -1,
      -1,    30,    31,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    33,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    13,    14,    15,    16,    17,    18,    19,    20,    21
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    36,     0,     3,     7,    25,    26,    27,    30,    31,
      37,    38,    42,    44,    45,    52,    53,     3,     6,    41,
      41,     9,     6,     4,     5,     6,    32,    39,    40,    41,
      41,    39,    38,    28,    29,    47,     8,    43,    39,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    46,    32,    55,    39,    48,    39,    38,    51,
      33,    39,    39,    39,    39,    39,    39,    39,    39,    39,
      39,    39,    39,    39,     9,    56,     9,    49,     9,    10,
      38,    51,    41,    57,    58,    54,     9,    51,    10,    33,
      34,    51,    51,    10,    58,    10,    10,    50,    47
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    35,    36,    36,    37,    37,    37,    38,    38,    38,
      38,    38,    38,    38,    39,    39,    39,    39,    39,    39,
      39,    39,    39,    39,    39,    39,    39,    39,    39,    40,
      40,    40,    40,    41,    43,    42,    44,    46,    45,    47,
      48,    47,    49,    50,    47,    51,    51,    51,    52,    52,
      54,    53,    56,    55,    57,    57,    57,    58
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     0,     2,     1,     4,     1,     2,
       1,     1,     2,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     1,
       1,     1,     1,     1,     0,     5,     2,     0,     6,     0,
       0,     5,     0,     0,     8,     0,     2,     1,     2,     2,
       0,     7,     0,     4,     0,     3,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: %empty  */
#line 81 "jackal_parser.y"
         { 
          program = jkl_program_new();
          program->symbol_table = jkl_hash_new();
          program->ast_prog_root = jkl_node_new(JKL_NODE_BLOCK);
          jkl_push_context(program, program->ast_prog_root);
        }
#line 1222 "jackal_parser.c"
    break;

  case 3: /* program: program statements  */
#line 87 "jackal_parser.y"
                            {
          jkl_pop_context(program);
          jkl_ensure_empty_contexts();
        }
#line 1231 "jackal_parser.c"
    break;

  case 7: /* statement: "let" ident ":=" expr  */
#line 98 "jackal_parser.y"
                                 {
            jkl_node_t* ident = (yyvsp[-2].node);
            jkl_node_t* expr = (yyvsp[0].node);

            if (jkl_get_context(program) == NULL)
              jkl_error("jkl_parser", "no current context");

            if (ident->type != JKL_NODE_ID) {
              yyerror("expected an identifier");
              YYERROR;
            } 

            jkl_node_t* let = jkl_node_new(JKL_NODE_LET);
            let->id = ident;
            let->expr = expr;

            jkl_log("jkl_parser", "emit statement: %p", let);
            jkl_node_append(jkl_get_context(program), let);
         }
#line 1255 "jackal_parser.c"
    break;

  case 9: /* statement: "raise" CSTRING  */
#line 118 "jackal_parser.y"
                         {
            jkl_node_t* raise = jkl_node_new(JKL_NODE_RAISE);
            raise->value.s = (yyvsp[0].string);

            if (jkl_get_context(program) == NULL)
              jkl_error("jkl_parser", "no current context");

            jkl_log("jkl_parser", "raise: %s", (yyvsp[0].string));

            jkl_node_append(jkl_get_context(program), raise);
         }
#line 1271 "jackal_parser.c"
    break;

  case 10: /* statement: call  */
#line 129 "jackal_parser.y"
                {
            jkl_note("jkl_parser", "emit ast call");
            jkl_node_t* call = (yyvsp[0].node);

            if (jkl_get_context(program) == NULL)
              jkl_error("jkl_parser", "no current context");

            jkl_log("jkl_parser", "call: %p", call);

            jkl_node_append(jkl_get_context(program), call);
          }
#line 1287 "jackal_parser.c"
    break;

  case 12: /* statement: "return" expr  */
#line 141 "jackal_parser.y"
                       {
            jkl_node_t* ret = jkl_node_new(JKL_NODE_RETURN);
            ret->expr = (yyvsp[0].node);

            if (jkl_get_context(program) == NULL)
              jkl_error("jkl_parser", "no current context");

            jkl_log("jkl_parser", "return: %p", ret);

            jkl_node_append(jkl_get_context(program), ret);
         }
#line 1303 "jackal_parser.c"
    break;

  case 14: /* expr: expr "+" expr  */
#line 155 "jackal_parser.y"
                      { (yyval.node) = jkl_node_binop((yyvsp[-2].node), JKL_OP_PLUS,  (yyvsp[0].node)); }
#line 1309 "jackal_parser.c"
    break;

  case 15: /* expr: expr "-" expr  */
#line 156 "jackal_parser.y"
                      { (yyval.node) = jkl_node_binop((yyvsp[-2].node), JKL_OP_MINUS, (yyvsp[0].node)); }
#line 1315 "jackal_parser.c"
    break;

  case 16: /* expr: expr "*" expr  */
#line 157 "jackal_parser.y"
                      { (yyval.node) = jkl_node_binop((yyvsp[-2].node), JKL_OP_MUL,   (yyvsp[0].node)); }
#line 1321 "jackal_parser.c"
    break;

  case 17: /* expr: expr "/" expr  */
#line 158 "jackal_parser.y"
                      { (yyval.node) = jkl_node_binop((yyvsp[-2].node), JKL_OP_DIV,   (yyvsp[0].node)); }
#line 1327 "jackal_parser.c"
    break;

  case 18: /* expr: expr "%" expr  */
#line 159 "jackal_parser.y"
                      { (yyval.node) = jkl_node_binop((yyvsp[-2].node), JKL_OP_MOD,   (yyvsp[0].node)); }
#line 1333 "jackal_parser.c"
    break;

  case 19: /* expr: expr "==" expr  */
#line 160 "jackal_parser.y"
                      { (yyval.node) = jkl_node_binop((yyvsp[-2].node), JKL_OP_EQL,   (yyvsp[0].node)); }
#line 1339 "jackal_parser.c"
    break;

  case 20: /* expr: expr "!=" expr  */
#line 161 "jackal_parser.y"
                      { (yyval.node) = jkl_node_binop((yyvsp[-2].node), JKL_OP_NEQ,   (yyvsp[0].node)); }
#line 1345 "jackal_parser.c"
    break;

  case 21: /* expr: expr ">" expr  */
#line 162 "jackal_parser.y"
                      { (yyval.node) = jkl_node_binop((yyvsp[-2].node), JKL_OP_GT,    (yyvsp[0].node)); }
#line 1351 "jackal_parser.c"
    break;

  case 22: /* expr: expr ">=" expr  */
#line 163 "jackal_parser.y"
                      { (yyval.node) = jkl_node_binop((yyvsp[-2].node), JKL_OP_GTE,   (yyvsp[0].node)); }
#line 1357 "jackal_parser.c"
    break;

  case 23: /* expr: expr "<" expr  */
#line 164 "jackal_parser.y"
                      { (yyval.node) = jkl_node_binop((yyvsp[-2].node), JKL_OP_LT,    (yyvsp[0].node)); }
#line 1363 "jackal_parser.c"
    break;

  case 24: /* expr: expr "<=" expr  */
#line 165 "jackal_parser.y"
                      { (yyval.node) = jkl_node_binop((yyvsp[-2].node), JKL_OP_LTE,   (yyvsp[0].node)); }
#line 1369 "jackal_parser.c"
    break;

  case 25: /* expr: expr "&&" expr  */
#line 166 "jackal_parser.y"
                      { (yyval.node) = jkl_node_binop((yyvsp[-2].node), JKL_OP_AND,   (yyvsp[0].node)); }
#line 1375 "jackal_parser.c"
    break;

  case 26: /* expr: expr "||" expr  */
#line 167 "jackal_parser.y"
                      { (yyval.node) = jkl_node_binop((yyvsp[-2].node), JKL_OP_OR,    (yyvsp[0].node)); }
#line 1381 "jackal_parser.c"
    break;

  case 27: /* expr: "(" expr ")"  */
#line 168 "jackal_parser.y"
                         { (yyval.node) = (yyvsp[-1].node); }
#line 1387 "jackal_parser.c"
    break;

  case 28: /* expr: term  */
#line 169 "jackal_parser.y"
                         { (yyval.node) = (yyvsp[0].node); }
#line 1393 "jackal_parser.c"
    break;

  case 30: /* term: CINT  */
#line 173 "jackal_parser.y"
           {
      jkl_node_t* cint = jkl_node_new(JKL_NODE_INT);
      cint->value.i = (yyvsp[0].number);

      jkl_log("jkl_parser", "emit ast cint: %d", (yyvsp[0].number));

      (yyval.node) = cint;
    }
#line 1406 "jackal_parser.c"
    break;

  case 31: /* term: CSTRING  */
#line 181 "jackal_parser.y"
              {
      jkl_node_t* cstring = jkl_node_new(JKL_NODE_STRING);
      cstring->value.s = (yyvsp[0].string);

      jkl_log("jkl_parser", "emit ast cstring: %s", (yyvsp[0].string));

      (yyval.node) = cstring;
    }
#line 1419 "jackal_parser.c"
    break;

  case 32: /* term: CFLOAT  */
#line 189 "jackal_parser.y"
             {
      jkl_node_t* cfloat = jkl_node_new(JKL_NODE_FLOAT);
      cfloat->value.f = (yyvsp[0].fnumber);

      jkl_log("jkl_parser", "emit ast cfloat: %f", (yyvsp[0].fnumber));

      (yyval.node) = cfloat;
    }
#line 1432 "jackal_parser.c"
    break;

  case 33: /* ident: ID  */
#line 199 "jackal_parser.y"
          {
        jkl_node_t* ident = jkl_node_new(JKL_NODE_ID);
        ident->value.s = (yyvsp[0].id);

        jkl_log("jkl_parser", "emit ast ident: %s", (yyvsp[0].id));

        (yyval.node) = ident;
      }
#line 1445 "jackal_parser.c"
    break;

  case 34: /* $@1: %empty  */
#line 209 "jackal_parser.y"
                  {
        jkl_note("jkl_parser", "begin emit ast loop");
        jkl_note("jkl_parser", "begin emit ast block");
        jkl_node_t* block = jkl_node_new(JKL_NODE_BLOCK);
        jkl_push_context(program, block);
      }
#line 1456 "jackal_parser.c"
    break;

  case 35: /* loop: "loop" "{" $@1 block_stmts "}"  */
#line 216 "jackal_parser.y"
             {
        jkl_note("jkl_parser", "end emit ast block");
        jkl_node_t* block = jkl_pop_context(program);

        jkl_node_t *loop = jkl_node_new(JKL_NODE_LOOP);
        loop->block = block;

        jkl_node_t *context = jkl_get_context(program);
        if (context == NULL)
          jkl_error("jkl_parser", "no current context");

        jkl_node_append(context, loop);
        jkl_log("jkl_parser", "emit ast loop");
      }
#line 1475 "jackal_parser.c"
    break;

  case 37: /* @2: %empty  */
#line 235 "jackal_parser.y"
                 {
          jkl_note("jkl_parser", "begin emit ast if");
          jkl_node_t* if_node = jkl_node_new(JKL_NODE_IF);
          if_node->expr = (yyvsp[0].node);
          if_node->block = jkl_node_new(JKL_NODE_BLOCK);
          jkl_node_append(jkl_get_context(program), if_node);

          jkl_push_context(program, if_node->block);
          jkl_note("jkl_parser", "begin emit ast if block");
          (yyval.node) = if_node;
        }
#line 1491 "jackal_parser.c"
    break;

  case 38: /* if_then: "if" expr @2 "{" block_stmts "}"  */
#line 245 "jackal_parser.y"
                                    {
          jkl_pop_context(program);
          jkl_note("jkl_parser", "end emit ast if then");
          (yyval.node) = (yyvsp[-3].node);
        }
#line 1501 "jackal_parser.c"
    break;

  case 40: /* $@3: %empty  */
#line 254 "jackal_parser.y"
             {
          jkl_node_t* if_node = (yyvsp[-1].node);
          if_node->block_else = jkl_node_new(JKL_NODE_BLOCK);
          jkl_push_context(program, if_node->block_else);
          jkl_note("jkl_parser", "begin emit ast else block");
        }
#line 1512 "jackal_parser.c"
    break;

  case 41: /* else_opt: "else" $@3 "{" block_stmts "}"  */
#line 259 "jackal_parser.y"
                                    {
          jkl_pop_context(program);
          jkl_note("jkl_parser", "end emit ast else");
        }
#line 1521 "jackal_parser.c"
    break;

  case 42: /* @4: %empty  */
#line 263 "jackal_parser.y"
                  {
          /* desugar `elif` into `else { if ... }` */
          jkl_node_t* outer = (yyvsp[-2].node);
          outer->block_else = jkl_node_new(JKL_NODE_BLOCK);
          jkl_push_context(program, outer->block_else);

          jkl_node_t* inner = jkl_node_new(JKL_NODE_IF);
          inner->expr = (yyvsp[0].node);
          inner->block = jkl_node_new(JKL_NODE_BLOCK);
          jkl_node_append(jkl_get_context(program), inner);
          jkl_push_context(program, inner->block);
          jkl_note("jkl_parser", "begin emit ast elif");
          (yyval.node) = inner;
        }
#line 1540 "jackal_parser.c"
    break;

  case 43: /* @5: %empty  */
#line 276 "jackal_parser.y"
                                    {
          jkl_pop_context(program);
          (yyval.node) = (yyvsp[-3].node);
        }
#line 1549 "jackal_parser.c"
    break;

  case 44: /* else_opt: "elif" expr @4 "{" block_stmts "}" @5 else_opt  */
#line 279 "jackal_parser.y"
                   {
          jkl_pop_context(program);
          jkl_note("jkl_parser", "end emit ast elif");
        }
#line 1558 "jackal_parser.c"
    break;

  case 48: /* call: ID CSTRING  */
#line 290 "jackal_parser.y"
                 {
      jkl_node_t* cstring = jkl_node_new(JKL_NODE_STRING);
      cstring->value.s = (yyvsp[0].string);

      jkl_node_t* call = jkl_node_new(JKL_NODE_CALL);
      call->node = cstring;

      jkl_log("jkl_parser", "emit ast call: %s", (yyvsp[0].string));

      (yyval.node) = call;
    }
#line 1574 "jackal_parser.c"
    break;

  case 49: /* call: ID ident  */
#line 301 "jackal_parser.y"
               {
      jkl_node_t* ident = (yyvsp[0].node);
      jkl_node_t* call = jkl_node_new(JKL_NODE_CALL);
      call->node = ident;

      jkl_log("jkl_parser", "emit ast call: %s", (yyvsp[0].node));

      (yyval.node) = call;
    }
#line 1588 "jackal_parser.c"
    break;

  case 50: /* $@6: %empty  */
#line 312 "jackal_parser.y"
                                    {
        jkl_node_t* block = jkl_node_new(JKL_NODE_BLOCK);
        jkl_push_context(program, block);
      }
#line 1597 "jackal_parser.c"
    break;

  case 51: /* func: "func" ident func_params "{" $@6 block_stmts "}"  */
#line 315 "jackal_parser.y"
                           {
        jkl_node_t* block = jkl_pop_context(program);
        jkl_node_t* func = jkl_node_new(JKL_NODE_FUNC);

        func->id = (yyvsp[-5].node);
        func->params = (yyvsp[-4].node);
        func->block = block;

        jkl_log("jkl_parser", "emit ast func: %s", (yyvsp[-5].node));

        (yyval.node) = func;
      }
#line 1614 "jackal_parser.c"
    break;

  case 52: /* $@7: %empty  */
#line 329 "jackal_parser.y"
                    {
            jkl_node_t* params = jkl_node_new(JKL_NODE_PARAMS);
            jkl_push_context(program, params);
           }
#line 1623 "jackal_parser.c"
    break;

  case 53: /* func_params: "(" $@7 params ")"  */
#line 332 "jackal_parser.y"
                           {
            (yyval.node) = jkl_pop_context(program);
           }
#line 1631 "jackal_parser.c"
    break;

  case 55: /* params: params "," param  */
#line 338 "jackal_parser.y"
                           {
        jkl_node_t* params = jkl_pop_context(program);
        jkl_node_append(params, (yyvsp[0].node));
        jkl_push_context(program, params);
      }
#line 1641 "jackal_parser.c"
    break;

  case 56: /* params: param  */
#line 343 "jackal_parser.y"
              {
        jkl_node_t* params = jkl_pop_context(program);
        jkl_node_append(params, (yyvsp[0].node));
        jkl_push_context(program, params);
      }
#line 1651 "jackal_parser.c"
    break;

  case 57: /* param: ident  */
#line 350 "jackal_parser.y"
             {
      jkl_node_t* param = jkl_node_new(JKL_NODE_PARAM);
      param->id = (yyvsp[0].node);

      jkl_log("jkl_parser", "emit ast param: %s", (yyvsp[0].node));

      (yyval.node) = param;
    }
#line 1664 "jackal_parser.c"
    break;


#line 1668 "jackal_parser.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 360 "jackal_parser.y"


int yyerror(char *s) {
  jkl_error("jkl_parser", "line %d: %s near '%s'", yylineno, s, yytext);
}

int yywrap(void) {
  return 1;
}
