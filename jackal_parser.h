/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_JACKAL_PARSER_H_INCLUDED
# define YY_YY_JACKAL_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ID = 258,                      /* ID  */
    CINT = 259,                    /* CINT  */
    CFLOAT = 260,                  /* CFLOAT  */
    CSTRING = 261,                 /* CSTRING  */
    LET = 262,                     /* "let"  */
    ASSIGN = 263,                  /* ":="  */
    LBRACE = 264,                  /* "{"  */
    RBRACE = 265,                  /* "}"  */
    EQL = 266,                     /* "=="  */
    NEQ = 267,                     /* "!="  */
    GT = 268,                      /* ">"  */
    LT = 269,                      /* "<"  */
    GTE = 270,                     /* ">="  */
    LTE = 271,                     /* "<="  */
    PLUS = 272,                    /* "+"  */
    MINUS = 273,                   /* "-"  */
    MUL = 274,                     /* "*"  */
    DIV = 275,                     /* "/"  */
    MOD = 276,                     /* "%"  */
    AND = 277,                     /* "&&"  */
    OR = 278,                      /* "||"  */
    NOT = 279,                     /* "!"  */
    LOOP = 280,                    /* "loop"  */
    RAISE = 281,                   /* "raise"  */
    IF = 282,                      /* "if"  */
    ELIF = 283,                    /* "elif"  */
    ELSE = 284,                    /* "else"  */
    FUNC = 285,                    /* "func"  */
    RETURN = 286,                  /* "return"  */
    LPAREN = 287,                  /* "("  */
    RPAREN = 288,                  /* ")"  */
    COMMA = 289                    /* ","  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define ID 258
#define CINT 259
#define CFLOAT 260
#define CSTRING 261
#define LET 262
#define ASSIGN 263
#define LBRACE 264
#define RBRACE 265
#define EQL 266
#define NEQ 267
#define GT 268
#define LT 269
#define GTE 270
#define LTE 271
#define PLUS 272
#define MINUS 273
#define MUL 274
#define DIV 275
#define MOD 276
#define AND 277
#define OR 278
#define NOT 279
#define LOOP 280
#define RAISE 281
#define IF 282
#define ELIF 283
#define ELSE 284
#define FUNC 285
#define RETURN 286
#define LPAREN 287
#define RPAREN 288
#define COMMA 289

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 16 "jackal_parser.y"

  char        *id;
  char        *string;
  int         number;
  float       fnumber;
  jkl_node_t  *node;
  jkl_op_t    op;

#line 144 "jackal_parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_JACKAL_PARSER_H_INCLUDED  */
