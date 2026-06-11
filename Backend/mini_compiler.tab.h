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

#ifndef YY_YY_MINI_COMPILER_TAB_H_INCLUDED
# define YY_YY_MINI_COMPILER_TAB_H_INCLUDED
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
    KW_INT = 258,                  /* KW_INT  */
    KW_FLOAT = 259,                /* KW_FLOAT  */
    KW_CHAR = 260,                 /* KW_CHAR  */
    KW_DOUBLE = 261,               /* KW_DOUBLE  */
    KW_LONG = 262,                 /* KW_LONG  */
    KW_SHORT = 263,                /* KW_SHORT  */
    KW_VOID = 264,                 /* KW_VOID  */
    KW_IF = 265,                   /* KW_IF  */
    KW_ELSE = 266,                 /* KW_ELSE  */
    KW_WHILE = 267,                /* KW_WHILE  */
    KW_FOR = 268,                  /* KW_FOR  */
    KW_DO = 269,                   /* KW_DO  */
    KW_RETURN = 270,               /* KW_RETURN  */
    KW_BREAK = 271,                /* KW_BREAK  */
    KW_CONTINUE = 272,             /* KW_CONTINUE  */
    KW_STRUCT = 273,               /* KW_STRUCT  */
    DIRECTIVE = 274,               /* DIRECTIVE  */
    OP_EQ = 275,                   /* OP_EQ  */
    OP_NEQ = 276,                  /* OP_NEQ  */
    OP_LTE = 277,                  /* OP_LTE  */
    OP_GTE = 278,                  /* OP_GTE  */
    OP_INC = 279,                  /* OP_INC  */
    OP_DEC = 280,                  /* OP_DEC  */
    OP_PLUS_EQ = 281,              /* OP_PLUS_EQ  */
    OP_MINUS_EQ = 282,             /* OP_MINUS_EQ  */
    OP_MULT_EQ = 283,              /* OP_MULT_EQ  */
    OP_DIV_EQ = 284,               /* OP_DIV_EQ  */
    OP_AND = 285,                  /* OP_AND  */
    OP_OR = 286,                   /* OP_OR  */
    OP_RSHIFT = 287,               /* OP_RSHIFT  */
    OP_LSHIFT = 288,               /* OP_LSHIFT  */
    ARROW = 289,                   /* ARROW  */
    IDENTIFIER = 290,              /* IDENTIFIER  */
    STRING_LIT = 291,              /* STRING_LIT  */
    INT_LIT = 292,                 /* INT_LIT  */
    FLOAT_LIT = 293,               /* FLOAT_LIT  */
    UMINUS = 294                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 260 "mini_compiler.y"

    int      ival;
    double   fval;
    char    *sval;
    int      dtype;   /* DataType enum value */

#line 110 "mini_compiler.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_MINI_COMPILER_TAB_H_INCLUDED  */
