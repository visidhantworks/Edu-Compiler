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
#line 7 "mini_compiler.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- ANSI colours ---- */
#define RED    "\033[1;31m"
#define YELLOW "\033[1;33m"
#define GREEN  "\033[1;32m"
#define CYAN   "\033[1;36m"
#define BLUE   "\033[1;34m"
#define RESET  "\033[0m"
#define BOLD   "\033[1m"

/* ---- shared counters (also used by lexer) ---- */
int line_number    = 1;
int lex_error_count = 0;
int warning_count  = 0;
int syn_error_count = 0;
int sem_error_count = 0;

/* ============================================================
   SYMBOL TABLE
   ============================================================ */
#define SYM_MAX 256

typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_DOUBLE,
               TYPE_LONG, TYPE_SHORT, TYPE_VOID, TYPE_UNKNOWN } DataType;
typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION
} SymbolKind;


typedef struct {
    char name[64];
    DataType type;
    SymbolKind kind;

    int declared_line;

    int initialized;
    int used;
    int scope;
} Symbol;


Symbol sym_table[SYM_MAX];
int    sym_count  = 0;
int    cur_scope  = 0;

/* Current declared type (set when a declaration starts) */
DataType cur_decl_type = TYPE_UNKNOWN;

/* ---- type helpers ---- */
const char *type_name(DataType t) {
    switch(t) {
        case TYPE_INT:    return "int";
        case TYPE_FLOAT:  return "float";
        case TYPE_CHAR:   return "char";
        case TYPE_DOUBLE: return "double";
        case TYPE_LONG:   return "long";
        case TYPE_SHORT:  return "short";
        case TYPE_VOID:   return "void";
        default:          return "unknown";
    }
}

/* ---- symbol table operations ---- */
Symbol *sym_find(const char *name) {
    /* Search innermost scope first */
    for (int i = sym_count - 1; i >= 0; i--)
        if (strcmp(sym_table[i].name, name) == 0 &&
            sym_table[i].scope <= cur_scope)
            return &sym_table[i];
    return NULL;
}

Symbol *sym_find_current_scope(const char *name) {
    for (int i = 0; i < sym_count; i++)
        if (strcmp(sym_table[i].name, name) == 0 &&
            sym_table[i].scope == cur_scope)
            return &sym_table[i];
    return NULL;
}

void sem_error(const char *what, const char *why, const char *fix) {
    char w1[52], w2[52], w3[52];
    snprintf(w1, sizeof(w1), "%-50s", what);
    snprintf(w2, sizeof(w2), "%-50s", why);
    snprintf(w3, sizeof(w3), "%-50s", fix);
    printf(RED
    "\n  +----------------------------------------------------+\n"
    "  |  SEMANTIC ERROR at Line %-3d                      |\n"
    "  +----------------------------------------------------+\n"
    "  |  What  : %s|\n"
    "  |  Why   : %s|\n"
    "  |  Fix   : %s|\n"
    "  +----------------------------------------------------+\n\n"
    RESET, line_number, w1, w2, w3);
    sem_error_count++;
}

void sem_warning(const char *what, const char *tip) {
    char w1[52], w2[52];
    snprintf(w1, sizeof(w1), "%-50s", what);
    snprintf(w2, sizeof(w2), "%-50s", tip);
    printf(YELLOW
    "\n  +----------------------------------------------------+\n"
    "  |  SEMANTIC WARNING at Line %-3d                   |\n"
    "  +----------------------------------------------------+\n"
    "  |  %s|\n"
    "  |  Tip: %s|\n"
    "  +----------------------------------------------------+\n\n"
    RESET, line_number, w1, w2);
    warning_count++;
}

/* Declare a variable; checks for duplicate in same scope */
void sym_declare(const char *name,DataType type,SymbolKind kind) {
    if (sym_find_current_scope(name)) {
        char msg[80], fix[80];
        snprintf(msg, sizeof(msg), "Redeclaration of '%s'", name);
        snprintf(fix, sizeof(fix), "Remove the second '%s %s;'", type_name(type), name);
        sem_error(msg, "Variable already declared in this scope", fix);
        return;
    }
    if (sym_count >= SYM_MAX) {
        sem_error("Symbol table full", "Too many variables", "Reduce variable count");
        return;
    }
    sym_table[sym_count].kind = kind;
    strncpy(sym_table[sym_count].name, name, 63);
    sym_table[sym_count].type          = type;
    sym_table[sym_count].declared_line = line_number;
    sym_table[sym_count].initialized   = 0;
    sym_table[sym_count].used          = 0;
    sym_table[sym_count].scope         = cur_scope;
    sym_count++;
    printf(GREEN "  [SEMANTIC] Declared: %s %s (scope %d)\n" RESET,
           type_name(type), name, cur_scope);
}

/* Mark variable as initialised (assigned a value) */
void sym_assign(const char *name) {
    Symbol *s = sym_find(name);
    if (!s) {
        char msg[80], fix[80];
        snprintf(msg, sizeof(msg), "Assignment to undeclared '%s'", name);
        snprintf(fix, sizeof(fix), "Declare it first: int %s;", name);
        sem_error(msg, "Variable was not declared before use", fix);
        return;
    }
    s->initialized = 1;
}

/* Mark variable as used (read in an expression) */
DataType sym_use(const char *name) {
    Symbol *s = sym_find(name);
    if (!s) {
        char msg[80], fix[80];
        snprintf(msg, sizeof(msg), "Use of undeclared variable '%s'", name);
        snprintf(fix, sizeof(fix), "Declare it first: int %s;", name);
        sem_error(msg, "Variable was not declared before use", fix);
        return TYPE_UNKNOWN;
    }
    if (!s->initialized) {
        char msg[80];
        snprintf(msg, sizeof(msg), "Variable '%s' used before initialisation", name);
        sem_warning(msg, "Always initialise variables, e.g.  int x = 0;");
    }
    s->used = 1;
    return s->type;
}

/* After a scope ends, warn about unused variables */
void sym_check_unused_scope(int scope) {
     
    for (int i = 0; i < sym_count; i++) {
        printf("DEBUG: %s kind=%d used=%d scope=%d\n",
               sym_table[i].name,
               sym_table[i].kind,
               sym_table[i].used,
               sym_table[i].scope);
        if (sym_table[i].kind == SYMBOL_VARIABLE &&sym_table[i].scope == scope &&!sym_table[i].used) {
            char msg[80];
            snprintf(msg, sizeof(msg), "Variable '%s' declared but never used",
                     sym_table[i].name);
            sem_warning(msg, "Remove it or use it to keep code clean");
        }
    }
}

/* Remove symbols belonging to a closed scope */
void sym_pop_scope(int scope) {
    sym_check_unused_scope(scope);
    int j = 0;
    for (int i = 0; i < sym_count; i++)
        if (sym_table[i].scope != scope)
            sym_table[j++] = sym_table[i];
    sym_count = j;
}

/* Print the whole symbol table */
void sym_print(void) {
    printf(BOLD "\n  SYMBOL TABLE\n" RESET);
    printf("  %-20s %-10s %-8s %-12s %-6s\n",
           "Name", "Type", "Scope", "Initialised", "Used");
    printf("  " CYAN "--------------------------------------------------------------\n" RESET);
    for (int i = 0; i < sym_count; i++) {
        printf("  %-20s %-10s %-8d %-12s %-6s\n",
               sym_table[i].name,
               type_name(sym_table[i].type),
               sym_table[i].scope,
               sym_table[i].initialized ? "yes" : "no",
               sym_table[i].used        ? "yes" : "no");
    }
}

/* ============================================================
   TYPE COMPATIBILITY  (for assignments / binary ops)
   ============================================================ */
int types_compatible(DataType lhs, DataType rhs) {
    if (lhs == TYPE_UNKNOWN || rhs == TYPE_UNKNOWN) return 1; /* already reported */
    if (lhs == rhs) return 1;
    /* Numeric promotions allowed */
    int numeric_l = (lhs==TYPE_INT||lhs==TYPE_FLOAT||lhs==TYPE_DOUBLE||
                     lhs==TYPE_LONG||lhs==TYPE_SHORT||lhs==TYPE_CHAR);
    int numeric_r = (rhs==TYPE_INT||rhs==TYPE_FLOAT||rhs==TYPE_DOUBLE||
                     rhs==TYPE_LONG||rhs==TYPE_SHORT||rhs==TYPE_CHAR);
    if (numeric_l && numeric_r) return 1;
    return 0;
}

/* ---- Bison error handler ---- */
void yyerror(const char *msg) {
    char w1[52];
    snprintf(w1, sizeof(w1), "%-50s", msg);
    printf(RED
    "\n  +----------------------------------------------------+\n"
    "  |  SYNTAX ERROR at Line %-3d                        |\n"
    "  +----------------------------------------------------+\n"
    "  |  %s|\n"
    "  +----------------------------------------------------+\n\n"
    RESET, line_number, w1);
    syn_error_count++;
}

extern int  yylex(void);
extern FILE *yyin;

#line 323 "mini_compiler.tab.c"

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

#include "mini_compiler.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_KW_INT = 3,                     /* KW_INT  */
  YYSYMBOL_KW_FLOAT = 4,                   /* KW_FLOAT  */
  YYSYMBOL_KW_CHAR = 5,                    /* KW_CHAR  */
  YYSYMBOL_KW_DOUBLE = 6,                  /* KW_DOUBLE  */
  YYSYMBOL_KW_LONG = 7,                    /* KW_LONG  */
  YYSYMBOL_KW_SHORT = 8,                   /* KW_SHORT  */
  YYSYMBOL_KW_VOID = 9,                    /* KW_VOID  */
  YYSYMBOL_KW_IF = 10,                     /* KW_IF  */
  YYSYMBOL_KW_ELSE = 11,                   /* KW_ELSE  */
  YYSYMBOL_KW_WHILE = 12,                  /* KW_WHILE  */
  YYSYMBOL_KW_FOR = 13,                    /* KW_FOR  */
  YYSYMBOL_KW_DO = 14,                     /* KW_DO  */
  YYSYMBOL_KW_RETURN = 15,                 /* KW_RETURN  */
  YYSYMBOL_KW_BREAK = 16,                  /* KW_BREAK  */
  YYSYMBOL_KW_CONTINUE = 17,               /* KW_CONTINUE  */
  YYSYMBOL_KW_STRUCT = 18,                 /* KW_STRUCT  */
  YYSYMBOL_DIRECTIVE = 19,                 /* DIRECTIVE  */
  YYSYMBOL_OP_EQ = 20,                     /* OP_EQ  */
  YYSYMBOL_OP_NEQ = 21,                    /* OP_NEQ  */
  YYSYMBOL_OP_LTE = 22,                    /* OP_LTE  */
  YYSYMBOL_OP_GTE = 23,                    /* OP_GTE  */
  YYSYMBOL_OP_INC = 24,                    /* OP_INC  */
  YYSYMBOL_OP_DEC = 25,                    /* OP_DEC  */
  YYSYMBOL_OP_PLUS_EQ = 26,                /* OP_PLUS_EQ  */
  YYSYMBOL_OP_MINUS_EQ = 27,               /* OP_MINUS_EQ  */
  YYSYMBOL_OP_MULT_EQ = 28,                /* OP_MULT_EQ  */
  YYSYMBOL_OP_DIV_EQ = 29,                 /* OP_DIV_EQ  */
  YYSYMBOL_OP_AND = 30,                    /* OP_AND  */
  YYSYMBOL_OP_OR = 31,                     /* OP_OR  */
  YYSYMBOL_OP_RSHIFT = 32,                 /* OP_RSHIFT  */
  YYSYMBOL_OP_LSHIFT = 33,                 /* OP_LSHIFT  */
  YYSYMBOL_ARROW = 34,                     /* ARROW  */
  YYSYMBOL_IDENTIFIER = 35,                /* IDENTIFIER  */
  YYSYMBOL_STRING_LIT = 36,                /* STRING_LIT  */
  YYSYMBOL_INT_LIT = 37,                   /* INT_LIT  */
  YYSYMBOL_FLOAT_LIT = 38,                 /* FLOAT_LIT  */
  YYSYMBOL_39_ = 39,                       /* '='  */
  YYSYMBOL_40_ = 40,                       /* '|'  */
  YYSYMBOL_41_ = 41,                       /* '^'  */
  YYSYMBOL_42_ = 42,                       /* '&'  */
  YYSYMBOL_43_ = 43,                       /* '<'  */
  YYSYMBOL_44_ = 44,                       /* '>'  */
  YYSYMBOL_45_ = 45,                       /* '+'  */
  YYSYMBOL_46_ = 46,                       /* '-'  */
  YYSYMBOL_47_ = 47,                       /* '*'  */
  YYSYMBOL_48_ = 48,                       /* '/'  */
  YYSYMBOL_49_ = 49,                       /* '%'  */
  YYSYMBOL_50_ = 50,                       /* '!'  */
  YYSYMBOL_51_ = 51,                       /* '~'  */
  YYSYMBOL_UMINUS = 52,                    /* UMINUS  */
  YYSYMBOL_53_ = 53,                       /* '.'  */
  YYSYMBOL_54_ = 54,                       /* '['  */
  YYSYMBOL_55_ = 55,                       /* '('  */
  YYSYMBOL_56_ = 56,                       /* ';'  */
  YYSYMBOL_57_ = 57,                       /* ')'  */
  YYSYMBOL_58_ = 58,                       /* ','  */
  YYSYMBOL_59_ = 59,                       /* '{'  */
  YYSYMBOL_60_ = 60,                       /* '}'  */
  YYSYMBOL_61_ = 61,                       /* ']'  */
  YYSYMBOL_YYACCEPT = 62,                  /* $accept  */
  YYSYMBOL_program = 63,                   /* program  */
  YYSYMBOL_top_level_item = 64,            /* top_level_item  */
  YYSYMBOL_directive = 65,                 /* directive  */
  YYSYMBOL_function_def = 66,              /* function_def  */
  YYSYMBOL_67_1 = 67,                      /* $@1  */
  YYSYMBOL_param_list_opt = 68,            /* param_list_opt  */
  YYSYMBOL_param_list = 69,                /* param_list  */
  YYSYMBOL_param_decl = 70,                /* param_decl  */
  YYSYMBOL_compound_stmt = 71,             /* compound_stmt  */
  YYSYMBOL_72_2 = 72,                      /* $@2  */
  YYSYMBOL_stmt_list = 73,                 /* stmt_list  */
  YYSYMBOL_statement = 74,                 /* statement  */
  YYSYMBOL_declaration = 75,               /* declaration  */
  YYSYMBOL_declarator_list = 76,           /* declarator_list  */
  YYSYMBOL_declarator_item = 77,           /* declarator_item  */
  YYSYMBOL_declarator = 78,                /* declarator  */
  YYSYMBOL_type_spec = 79,                 /* type_spec  */
  YYSYMBOL_expression_stmt = 80,           /* expression_stmt  */
  YYSYMBOL_if_stmt = 81,                   /* if_stmt  */
  YYSYMBOL_while_stmt = 82,                /* while_stmt  */
  YYSYMBOL_for_stmt = 83,                  /* for_stmt  */
  YYSYMBOL_for_init = 84,                  /* for_init  */
  YYSYMBOL_do_while_stmt = 85,             /* do_while_stmt  */
  YYSYMBOL_return_stmt = 86,               /* return_stmt  */
  YYSYMBOL_break_stmt = 87,                /* break_stmt  */
  YYSYMBOL_continue_stmt = 88,             /* continue_stmt  */
  YYSYMBOL_expression = 89,                /* expression  */
  YYSYMBOL_unary_expr = 90,                /* unary_expr  */
  YYSYMBOL_primary_expr = 91,              /* primary_expr  */
  YYSYMBOL_arg_list_opt = 92,              /* arg_list_opt  */
  YYSYMBOL_arg_list = 93                   /* arg_list  */
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
typedef yytype_uint8 yy_state_t;

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
#define YYLAST   949

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  32
/* YYNRULES -- Number of rules.  */
#define YYNRULES  114
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  212

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   294


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
       2,     2,     2,    50,     2,     2,     2,    49,    42,     2,
      55,    57,    47,    45,    58,    46,    53,    48,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    56,
      43,    39,    44,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    54,     2,    61,    41,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    59,    40,    60,    51,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    52
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   305,   305,   306,   310,   311,   312,   313,   318,   319,
     320,   328,   327,   350,   351,   355,   356,   360,   374,   382,
     381,   392,   393,   397,   398,   399,   400,   401,   402,   403,
     404,   405,   406,   407,   408,   413,   417,   418,   422,   426,
     446,   458,   459,   463,   464,   465,   466,   467,   468,   469,
     474,   475,   483,   484,   485,   490,   491,   496,   497,   498,
     499,   503,   504,   505,   510,   515,   516,   519,   520,   526,
     527,   543,   544,   545,   546,   547,   548,   549,   550,   551,
     552,   553,   554,   555,   564,   565,   566,   574,   575,   576,
     577,   578,   579,   583,   584,   585,   586,   587,   588,   589,
     590,   591,   592,   596,   597,   598,   599,   605,   613,   621,
     622,   633,   634,   638,   639
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
  "\"end of file\"", "error", "\"invalid token\"", "KW_INT", "KW_FLOAT",
  "KW_CHAR", "KW_DOUBLE", "KW_LONG", "KW_SHORT", "KW_VOID", "KW_IF",
  "KW_ELSE", "KW_WHILE", "KW_FOR", "KW_DO", "KW_RETURN", "KW_BREAK",
  "KW_CONTINUE", "KW_STRUCT", "DIRECTIVE", "OP_EQ", "OP_NEQ", "OP_LTE",
  "OP_GTE", "OP_INC", "OP_DEC", "OP_PLUS_EQ", "OP_MINUS_EQ", "OP_MULT_EQ",
  "OP_DIV_EQ", "OP_AND", "OP_OR", "OP_RSHIFT", "OP_LSHIFT", "ARROW",
  "IDENTIFIER", "STRING_LIT", "INT_LIT", "FLOAT_LIT", "'='", "'|'", "'^'",
  "'&'", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'~'",
  "UMINUS", "'.'", "'['", "'('", "';'", "')'", "','", "'{'", "'}'", "']'",
  "$accept", "program", "top_level_item", "directive", "function_def",
  "$@1", "param_list_opt", "param_list", "param_decl", "compound_stmt",
  "$@2", "stmt_list", "statement", "declaration", "declarator_list",
  "declarator_item", "declarator", "type_spec", "expression_stmt",
  "if_stmt", "while_stmt", "for_stmt", "for_init", "do_while_stmt",
  "return_stmt", "break_stmt", "continue_stmt", "expression", "unary_expr",
  "primary_expr", "arg_list_opt", "arg_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-111)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -111,   930,  -111,   -54,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,   -13,  -111,  -111,  -111,   -52,   -32,  -111,  -111,  -111,
      -7,  -111,   -12,     7,   -14,  -111,   -33,     2,   189,  -111,
      -6,   665,    35,  -111,    19,    20,  -111,    45,  -111,  -111,
      46,    54,   -15,  -111,  -111,  -111,    56,   689,    63,   689,
     689,   307,   725,  -111,  -111,    41,  -111,   189,  -111,  -111,
    -111,  -111,  -111,   665,   665,   665,   665,   665,   665,   665,
    -111,    58,  -111,  -111,  -111,  -111,    43,   405,   665,   665,
     665,   665,   665,   665,   665,   665,   665,   665,   665,   665,
     665,   665,   665,   665,   665,   665,  -111,    44,  -111,   725,
     725,   725,   725,   725,   343,   725,    51,    52,   689,  -111,
     880,   880,   251,   251,   785,   755,    39,    39,   815,   845,
     852,   251,   251,   -22,   -22,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,   665,  -111,  -111,   725,   166,    53,    15,    16,
      18,   226,   102,    68,    69,  -111,  -111,  -111,  -111,    72,
      -6,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,    74,
    -111,    75,   665,    77,   665,    79,   283,   117,  -111,   585,
    -111,  -111,  -111,  -111,  -111,   226,   435,   226,   465,   226,
    -111,    85,   641,   615,    87,  -111,  -111,   226,  -111,   226,
    -111,  -111,   665,   375,  -111,   665,   132,  -111,   495,   665,
     226,   525,   226,   226,   555,  -111,    89,  -111,  -111,   226,
    -111,  -111
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,     0,    43,    44,    45,    46,    47,    48,
      49,     0,     3,     4,     5,     0,     0,     7,     8,    10,
       0,     6,    41,     0,    35,    36,    38,     0,    13,    42,
       0,     0,     0,     9,     0,    14,    15,    18,    41,    37,
       0,     0,   106,   105,   103,   104,     0,     0,     0,     0,
       0,     0,    39,    69,    93,     0,    11,     0,    17,    97,
      98,    99,   100,     0,     0,     0,     0,     0,     0,   111,
     101,   106,    94,   102,    95,    96,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,     0,    16,    71,
      72,    73,    74,    70,     0,   113,     0,   112,     0,   109,
      77,    78,    79,    80,    75,    76,    89,    88,    90,    92,
      91,    81,    82,    83,    84,    85,    86,    87,    19,    12,
     108,   107,     0,   110,    21,   114,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    33,    20,    25,    22,     0,
       0,    24,    26,    27,    28,    29,    30,    31,    32,     0,
      34,     0,     0,     0,     0,     0,     0,     0,    66,     0,
      67,    68,    23,    51,    50,     0,     0,     0,     0,     0,
      61,     0,     0,     0,     0,    65,    54,     0,    56,     0,
      60,    63,     0,     0,    62,     0,    52,    55,     0,     0,
       0,     0,     0,     0,     0,    59,     0,    53,    58,     0,
      64,    57
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,    90,    49,
    -111,  -111,  -110,     0,  -111,   120,  -111,    17,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,   -31,   -42,  -111,
    -111,  -111
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,    12,    13,    14,    97,    34,    35,    36,   147,
     134,   136,   148,   149,    24,    25,    26,   150,   151,   152,
     153,   154,   182,   155,   156,   157,   158,   159,    53,    54,
     106,   107
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      52,    15,    17,    22,    21,    72,    31,    74,    75,    61,
      62,    63,    64,    65,    66,    23,   161,   163,    16,   165,
      77,    32,    18,    19,    67,    93,    94,    95,    27,    38,
      20,   167,    99,   100,   101,   102,   103,   104,   105,    68,
      69,    23,    29,    28,    30,    37,    33,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   186,   133,   188,    76,   190,
     162,   164,    55,   166,    37,   173,    56,   196,    57,   197,
      58,    59,    61,    62,    91,    92,    93,    94,    95,    60,
     205,    70,   207,   208,    78,    79,    80,    81,    73,   211,
     108,   135,    96,   128,    82,    83,    84,    85,   131,   160,
     132,   169,    68,    69,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,   170,   171,    40,    41,   172,   184,
     174,   176,   175,   178,   177,   183,   179,    42,    43,    44,
      45,   191,   195,   202,    46,   210,   129,    98,    47,    48,
      39,   193,    49,    50,     0,     0,     0,    51,   168,     0,
       0,   198,     0,     0,   201,     0,   181,   137,   204,     4,
       5,     6,     7,     8,     9,    10,   138,     0,   139,   140,
     141,   142,   143,   144,     0,     0,     0,     0,     0,     0,
      40,    41,     4,     5,     6,     7,     8,     9,    10,     0,
       0,    42,    43,    44,    45,     0,     0,     0,    46,     0,
       0,     0,    47,    48,     0,     0,    49,    50,     0,     0,
       0,    51,   145,     0,     0,   128,   146,   137,     0,     4,
       5,     6,     7,     8,     9,    10,   138,     0,   139,   140,
     141,   142,   143,   144,     0,     0,     0,     0,     0,     0,
      40,    41,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    42,    43,    44,    45,     0,     0,     0,    46,     0,
       0,     0,    47,    48,     0,     0,    49,    50,     0,     0,
       0,    51,   145,    84,    85,   128,     4,     5,     6,     7,
       8,     9,    10,     0,     0,     0,    91,    92,    93,    94,
      95,     0,     0,     0,     0,     0,     0,    40,    41,     0,
       4,     5,     6,     7,     8,     9,    10,     0,    42,    43,
      44,    45,     0,     0,     0,    46,     0,     0,     0,    47,
      48,    40,    41,    49,    50,     0,     0,     0,    51,   180,
       0,     0,    42,    43,    44,    45,     0,     0,     0,    46,
       0,     0,     0,    47,    48,     0,     0,    49,    50,     0,
       0,     0,    51,    78,    79,    80,    81,     0,     0,     0,
       0,     0,     0,    82,    83,    84,    85,     0,     0,     0,
       0,     0,     0,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,     0,     0,    78,    79,    80,    81,     0,
       0,     0,     0,     0,   130,    82,    83,    84,    85,     0,
       0,     0,     0,     0,     0,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    78,    79,    80,    81,     0,
       0,   199,   200,     0,     0,    82,    83,    84,    85,     0,
       0,     0,     0,     0,     0,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    78,    79,    80,    81,     0,
       0,     0,   109,     0,     0,    82,    83,    84,    85,     0,
       0,     0,     0,     0,     0,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    78,    79,    80,    81,     0,
       0,     0,   187,     0,     0,    82,    83,    84,    85,     0,
       0,     0,     0,     0,     0,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    78,    79,    80,    81,     0,
       0,     0,   189,     0,     0,    82,    83,    84,    85,     0,
       0,     0,     0,     0,     0,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    78,    79,    80,    81,     0,
       0,     0,   203,     0,     0,    82,    83,    84,    85,     0,
       0,     0,     0,     0,     0,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    78,    79,    80,    81,     0,
       0,     0,   206,     0,     0,    82,    83,    84,    85,     0,
       0,     0,     0,     0,     0,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    78,    79,    80,    81,     0,
       0,     0,   209,     0,     0,    82,    83,    84,    85,     0,
       0,     0,     0,     0,     0,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    78,    79,    80,    81,     0,
       0,   185,     0,     0,     0,    82,    83,    84,    85,     0,
       0,     0,     0,     0,     0,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    40,    41,     0,     0,     0,
       0,   194,     0,     0,     0,     0,    42,    43,    44,    45,
       0,     0,     0,    46,     0,     0,     0,    47,    48,    40,
      41,    49,    50,     0,     0,     0,    51,   192,     0,     0,
      42,    43,    44,    45,     0,     0,     0,    46,     0,     0,
       0,    47,    48,    40,    41,    49,    50,     0,     0,     0,
      51,     0,     0,     0,    71,    43,    44,    45,     0,     0,
       0,    46,     0,     0,     0,    47,    48,     0,     0,    49,
      50,     0,     0,     0,    51,    78,    79,    80,    81,     0,
       0,     0,     0,     0,     0,    82,    83,    84,    85,     0,
       0,     0,     0,     0,     0,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    78,    79,    80,    81,     0,
       0,     0,     0,     0,     0,    82,     0,    84,    85,     0,
       0,     0,     0,     0,     0,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    78,    79,    80,    81,     0,
       0,     0,     0,     0,     0,     0,     0,    84,    85,     0,
       0,     0,     0,     0,     0,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    78,    79,    80,    81,     0,
       0,     0,     0,     0,     0,     0,     0,    84,    85,     0,
       0,     0,     0,     0,     0,     0,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    78,    79,    80,    81,     0,
       0,     0,    78,    79,    80,    81,     0,    84,    85,     0,
       0,     0,     0,     0,    84,    85,     0,    88,    89,    90,
      91,    92,    93,    94,    95,    89,    90,    91,    92,    93,
      94,    95,    80,    81,     0,     0,     0,     0,     0,     0,
       0,     0,    84,    85,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    89,    90,    91,    92,    93,    94,    95,
       2,     3,     0,     4,     5,     6,     7,     8,     9,    10,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    11
};

static const yytype_int16 yycheck[] =
{
      31,     1,    56,    35,    56,    47,    39,    49,    50,    24,
      25,    26,    27,    28,    29,    47,     1,     1,     1,     1,
      51,    54,    35,    36,    39,    47,    48,    49,    35,    35,
      43,   141,    63,    64,    65,    66,    67,    68,    69,    54,
      55,    47,    35,    55,    58,    28,    44,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,   175,   108,   177,    51,   179,
      55,    55,    37,    55,    57,     1,    57,   187,    58,   189,
      35,    35,    24,    25,    45,    46,    47,    48,    49,    35,
     200,    35,   202,   203,    20,    21,    22,    23,    35,   209,
      57,   132,    61,    59,    30,    31,    32,    33,    57,    56,
      58,   142,    54,    55,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    56,    56,    24,    25,    56,    12,
      56,   162,    57,   164,    57,   166,    57,    35,    36,    37,
      38,    56,    55,    11,    42,    56,    97,    57,    46,    47,
      30,   182,    50,    51,    -1,    -1,    -1,    55,    56,    -1,
      -1,   192,    -1,    -1,   195,    -1,   166,     1,   199,     3,
       4,     5,     6,     7,     8,     9,    10,    -1,    12,    13,
      14,    15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      24,    25,     3,     4,     5,     6,     7,     8,     9,    -1,
      -1,    35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,
      -1,    -1,    46,    47,    -1,    -1,    50,    51,    -1,    -1,
      -1,    55,    56,    -1,    -1,    59,    60,     1,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    -1,    12,    13,
      14,    15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      24,    25,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,
      -1,    -1,    46,    47,    -1,    -1,    50,    51,    -1,    -1,
      -1,    55,    56,    32,    33,    59,     3,     4,     5,     6,
       7,     8,     9,    -1,    -1,    -1,    45,    46,    47,    48,
      49,    -1,    -1,    -1,    -1,    -1,    -1,    24,    25,    -1,
       3,     4,     5,     6,     7,     8,     9,    -1,    35,    36,
      37,    38,    -1,    -1,    -1,    42,    -1,    -1,    -1,    46,
      47,    24,    25,    50,    51,    -1,    -1,    -1,    55,    56,
      -1,    -1,    35,    36,    37,    38,    -1,    -1,    -1,    42,
      -1,    -1,    -1,    46,    47,    -1,    -1,    50,    51,    -1,
      -1,    -1,    55,    20,    21,    22,    23,    -1,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    -1,    -1,    -1,
      -1,    -1,    -1,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    -1,    -1,    20,    21,    22,    23,    -1,
      -1,    -1,    -1,    -1,    61,    30,    31,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    20,    21,    22,    23,    -1,
      -1,    56,    57,    -1,    -1,    30,    31,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    20,    21,    22,    23,    -1,
      -1,    -1,    57,    -1,    -1,    30,    31,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    20,    21,    22,    23,    -1,
      -1,    -1,    57,    -1,    -1,    30,    31,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    20,    21,    22,    23,    -1,
      -1,    -1,    57,    -1,    -1,    30,    31,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    20,    21,    22,    23,    -1,
      -1,    -1,    57,    -1,    -1,    30,    31,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    20,    21,    22,    23,    -1,
      -1,    -1,    57,    -1,    -1,    30,    31,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    20,    21,    22,    23,    -1,
      -1,    -1,    57,    -1,    -1,    30,    31,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    20,    21,    22,    23,    -1,
      -1,    56,    -1,    -1,    -1,    30,    31,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    24,    25,    -1,    -1,    -1,
      -1,    56,    -1,    -1,    -1,    -1,    35,    36,    37,    38,
      -1,    -1,    -1,    42,    -1,    -1,    -1,    46,    47,    24,
      25,    50,    51,    -1,    -1,    -1,    55,    56,    -1,    -1,
      35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,    -1,
      -1,    46,    47,    24,    25,    50,    51,    -1,    -1,    -1,
      55,    -1,    -1,    -1,    35,    36,    37,    38,    -1,    -1,
      -1,    42,    -1,    -1,    -1,    46,    47,    -1,    -1,    50,
      51,    -1,    -1,    -1,    55,    20,    21,    22,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    20,    21,    22,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    30,    -1,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    20,    21,    22,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    20,    21,    22,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    20,    21,    22,    23,    -1,
      -1,    -1,    20,    21,    22,    23,    -1,    32,    33,    -1,
      -1,    -1,    -1,    -1,    32,    33,    -1,    42,    43,    44,
      45,    46,    47,    48,    49,    43,    44,    45,    46,    47,
      48,    49,    22,    23,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    33,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    48,    49,
       0,     1,    -1,     3,     4,     5,     6,     7,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    63,     0,     1,     3,     4,     5,     6,     7,     8,
       9,    19,    64,    65,    66,    75,    79,    56,    35,    36,
      43,    56,    35,    47,    76,    77,    78,    35,    55,    35,
      58,    39,    54,    44,    68,    69,    70,    79,    35,    77,
      24,    25,    35,    36,    37,    38,    42,    46,    47,    50,
      51,    55,    89,    90,    91,    37,    57,    58,    35,    35,
      35,    24,    25,    26,    27,    28,    29,    39,    54,    55,
      35,    35,    90,    35,    90,    90,    79,    89,    20,    21,
      22,    23,    30,    31,    32,    33,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    61,    67,    70,    89,
      89,    89,    89,    89,    89,    89,    92,    93,    57,    57,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    59,    71,
      61,    57,    58,    90,    72,    89,    73,     1,    10,    12,
      13,    14,    15,    16,    17,    56,    60,    71,    74,    75,
      79,    80,    81,    82,    83,    85,    86,    87,    88,    89,
      56,     1,    55,     1,    55,     1,    55,    74,    56,    89,
      56,    56,    56,     1,    56,    57,    89,    57,    89,    57,
      56,    75,    84,    89,    12,    56,    74,    57,    74,    57,
      74,    56,    56,    89,    56,    55,    74,    74,    89,    56,
      57,    89,    11,    57,    89,    74,    57,    74,    74,    57,
      56,    74
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    62,    63,    63,    64,    64,    64,    64,    65,    65,
      65,    67,    66,    68,    68,    69,    69,    70,    70,    72,
      71,    73,    73,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    75,    76,    76,    77,    77,
      77,    78,    78,    79,    79,    79,    79,    79,    79,    79,
      80,    80,    81,    81,    81,    82,    82,    83,    83,    83,
      83,    84,    84,    84,    85,    86,    86,    87,    88,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    91,    91,    91,    91,    91,    91,    91,
      91,    92,    92,    93,    93
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     2,     2,     2,     4,
       2,     0,     7,     0,     1,     1,     3,     2,     1,     0,
       4,     0,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     1,     3,     1,     3,
       4,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     5,     7,     4,     5,     4,     8,     7,     6,
       4,     1,     2,     2,     7,     3,     2,     2,     2,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     1,     1,     1,     4,     4,     3,
       4,     0,     1,     1,     3
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
  case 7: /* top_level_item: error ';'  */
#line 313 "mini_compiler.y"
                 { yyerrok; }
#line 1697 "mini_compiler.tab.c"
    break;

  case 8: /* directive: DIRECTIVE IDENTIFIER  */
#line 318 "mini_compiler.y"
                                     { free((yyvsp[0].sval)); }
#line 1703 "mini_compiler.tab.c"
    break;

  case 9: /* directive: DIRECTIVE '<' IDENTIFIER '>'  */
#line 319 "mini_compiler.y"
                                     { free((yyvsp[-1].sval)); }
#line 1709 "mini_compiler.tab.c"
    break;

  case 10: /* directive: DIRECTIVE STRING_LIT  */
#line 320 "mini_compiler.y"
                                     { free((yyvsp[0].sval)); }
#line 1715 "mini_compiler.tab.c"
    break;

  case 11: /* $@1: %empty  */
#line 328 "mini_compiler.y"
        {
            sym_declare((yyvsp[-3].sval),
                        (DataType)(yyvsp[-4].dtype),
                        SYMBOL_FUNCTION);

            Symbol *s = sym_find((yyvsp[-3].sval));

            if (s)
                s->initialized = 1;

            free((yyvsp[-3].sval));

            cur_scope++;
        }
#line 1734 "mini_compiler.tab.c"
    break;

  case 12: /* function_def: type_spec IDENTIFIER '(' param_list_opt ')' $@1 compound_stmt  */
#line 343 "mini_compiler.y"
        {
            sym_pop_scope(cur_scope);
            cur_scope--;
        }
#line 1743 "mini_compiler.tab.c"
    break;

  case 17: /* param_decl: type_spec IDENTIFIER  */
#line 361 "mini_compiler.y"
        {
            sym_declare((yyvsp[0].sval),
                        (DataType)(yyvsp[-1].dtype),
                        SYMBOL_VARIABLE);

            /* parameters are pre-initialised by caller */
            Symbol *s = sym_find((yyvsp[0].sval));

            if (s)
                s->initialized = 1;

            free((yyvsp[0].sval));
        }
#line 1761 "mini_compiler.tab.c"
    break;

  case 19: /* $@2: %empty  */
#line 382 "mini_compiler.y"
        { cur_scope++; }
#line 1767 "mini_compiler.tab.c"
    break;

  case 20: /* compound_stmt: '{' $@2 stmt_list '}'  */
#line 385 "mini_compiler.y"
        {
            sym_pop_scope(cur_scope);
            cur_scope--;
        }
#line 1776 "mini_compiler.tab.c"
    break;

  case 34: /* statement: error ';'  */
#line 408 "mini_compiler.y"
                  { yyerrok; }
#line 1782 "mini_compiler.tab.c"
    break;

  case 38: /* declarator_item: declarator  */
#line 423 "mini_compiler.y"
        {sym_declare((yyvsp[0].sval),
            cur_decl_type,
            SYMBOL_VARIABLE); }
#line 1790 "mini_compiler.tab.c"
    break;

  case 39: /* declarator_item: declarator '=' expression  */
#line 427 "mini_compiler.y"
        {
            sym_declare((yyvsp[-2].sval),
            cur_decl_type,
            SYMBOL_VARIABLE);

            sym_assign((yyvsp[-2].sval));
            sym_assign((yyvsp[-2].sval));
            /* type check: cur_decl_type vs expression type ($3) */
            if (!types_compatible(cur_decl_type, (DataType)(yyvsp[0].dtype))) {
                char msg[80];
                snprintf(msg, sizeof(msg),
                         "Type mismatch: assigning '%s' to '%s %s'",
                         type_name((DataType)(yyvsp[0].dtype)),
                         type_name(cur_decl_type), (yyvsp[-2].sval));
                sem_error(msg, "Types are incompatible",
                          "Use a cast or change the variable type");
            }
            free((yyvsp[-2].sval));
        }
#line 1814 "mini_compiler.tab.c"
    break;

  case 40: /* declarator_item: declarator '[' INT_LIT ']'  */
#line 447 "mini_compiler.y"
        {
            char arr_name[80];
            snprintf(arr_name, sizeof(arr_name), "%s[%d]", (yyvsp[-3].sval), (yyvsp[-1].ival));
            sym_declare(arr_name,
            cur_decl_type,
            SYMBOL_VARIABLE);
            free((yyvsp[-3].sval));
        }
#line 1827 "mini_compiler.tab.c"
    break;

  case 41: /* declarator: IDENTIFIER  */
#line 458 "mini_compiler.y"
                  { (yyval.sval) = (yyvsp[0].sval); }
#line 1833 "mini_compiler.tab.c"
    break;

  case 42: /* declarator: '*' IDENTIFIER  */
#line 459 "mini_compiler.y"
                     { /* pointer */ (yyval.sval) = (yyvsp[0].sval); }
#line 1839 "mini_compiler.tab.c"
    break;

  case 43: /* type_spec: KW_INT  */
#line 463 "mini_compiler.y"
                { cur_decl_type = TYPE_INT;    (yyval.dtype) = TYPE_INT;    }
#line 1845 "mini_compiler.tab.c"
    break;

  case 44: /* type_spec: KW_FLOAT  */
#line 464 "mini_compiler.y"
                { cur_decl_type = TYPE_FLOAT;  (yyval.dtype) = TYPE_FLOAT;  }
#line 1851 "mini_compiler.tab.c"
    break;

  case 45: /* type_spec: KW_CHAR  */
#line 465 "mini_compiler.y"
                { cur_decl_type = TYPE_CHAR;   (yyval.dtype) = TYPE_CHAR;   }
#line 1857 "mini_compiler.tab.c"
    break;

  case 46: /* type_spec: KW_DOUBLE  */
#line 466 "mini_compiler.y"
                { cur_decl_type = TYPE_DOUBLE; (yyval.dtype) = TYPE_DOUBLE; }
#line 1863 "mini_compiler.tab.c"
    break;

  case 47: /* type_spec: KW_LONG  */
#line 467 "mini_compiler.y"
                { cur_decl_type = TYPE_LONG;   (yyval.dtype) = TYPE_LONG;   }
#line 1869 "mini_compiler.tab.c"
    break;

  case 48: /* type_spec: KW_SHORT  */
#line 468 "mini_compiler.y"
                { cur_decl_type = TYPE_SHORT;  (yyval.dtype) = TYPE_SHORT;  }
#line 1875 "mini_compiler.tab.c"
    break;

  case 49: /* type_spec: KW_VOID  */
#line 469 "mini_compiler.y"
                { cur_decl_type = TYPE_VOID;   (yyval.dtype) = TYPE_VOID;   }
#line 1881 "mini_compiler.tab.c"
    break;

  case 51: /* expression_stmt: expression error  */
#line 475 "mini_compiler.y"
                        { yyerrok;
                          sem_error("Missing semicolon",
                                    "Every C statement must end with ';'",
                                    "Add ';' at the end of this statement"); }
#line 1890 "mini_compiler.tab.c"
    break;

  case 54: /* if_stmt: KW_IF error ')' statement  */
#line 485 "mini_compiler.y"
                                  { yyerrok; }
#line 1896 "mini_compiler.tab.c"
    break;

  case 56: /* while_stmt: KW_WHILE error ')' statement  */
#line 491 "mini_compiler.y"
                                   { yyerrok; }
#line 1902 "mini_compiler.tab.c"
    break;

  case 60: /* for_stmt: KW_FOR error ')' statement  */
#line 499 "mini_compiler.y"
                                 { yyerrok; }
#line 1908 "mini_compiler.tab.c"
    break;

  case 70: /* expression: IDENTIFIER '=' expression  */
#line 528 "mini_compiler.y"
        {
            sym_assign((yyvsp[-2].sval));
            Symbol *s = sym_find((yyvsp[-2].sval));
            DataType ltype = s ? s->type : TYPE_UNKNOWN;
            if (!types_compatible(ltype, (DataType)(yyvsp[0].dtype))) {
                char msg[80];
                snprintf(msg, sizeof(msg),
                         "Type mismatch: assigning '%s' to '%s'",
                         type_name((DataType)(yyvsp[0].dtype)), type_name(ltype));
                sem_error(msg, "Incompatible types in assignment",
                          "Use explicit cast or change variable type");
            }
            free((yyvsp[-2].sval));
            (yyval.dtype) = ltype;
        }
#line 1928 "mini_compiler.tab.c"
    break;

  case 71: /* expression: IDENTIFIER OP_PLUS_EQ expression  */
#line 543 "mini_compiler.y"
                                        { sym_assign((yyvsp[-2].sval)); free((yyvsp[-2].sval)); (yyval.dtype) = TYPE_INT; }
#line 1934 "mini_compiler.tab.c"
    break;

  case 72: /* expression: IDENTIFIER OP_MINUS_EQ expression  */
#line 544 "mini_compiler.y"
                                        { sym_assign((yyvsp[-2].sval)); free((yyvsp[-2].sval)); (yyval.dtype) = TYPE_INT; }
#line 1940 "mini_compiler.tab.c"
    break;

  case 73: /* expression: IDENTIFIER OP_MULT_EQ expression  */
#line 545 "mini_compiler.y"
                                        { sym_assign((yyvsp[-2].sval)); free((yyvsp[-2].sval)); (yyval.dtype) = TYPE_INT; }
#line 1946 "mini_compiler.tab.c"
    break;

  case 74: /* expression: IDENTIFIER OP_DIV_EQ expression  */
#line 546 "mini_compiler.y"
                                        { sym_assign((yyvsp[-2].sval)); free((yyvsp[-2].sval)); (yyval.dtype) = TYPE_INT; }
#line 1952 "mini_compiler.tab.c"
    break;

  case 75: /* expression: expression OP_AND expression  */
#line 547 "mini_compiler.y"
                                   { (yyval.dtype) = TYPE_INT; }
#line 1958 "mini_compiler.tab.c"
    break;

  case 76: /* expression: expression OP_OR expression  */
#line 548 "mini_compiler.y"
                                   { (yyval.dtype) = TYPE_INT; }
#line 1964 "mini_compiler.tab.c"
    break;

  case 77: /* expression: expression OP_EQ expression  */
#line 549 "mini_compiler.y"
                                   { (yyval.dtype) = TYPE_INT; }
#line 1970 "mini_compiler.tab.c"
    break;

  case 78: /* expression: expression OP_NEQ expression  */
#line 550 "mini_compiler.y"
                                   { (yyval.dtype) = TYPE_INT; }
#line 1976 "mini_compiler.tab.c"
    break;

  case 79: /* expression: expression OP_LTE expression  */
#line 551 "mini_compiler.y"
                                   { (yyval.dtype) = TYPE_INT; }
#line 1982 "mini_compiler.tab.c"
    break;

  case 80: /* expression: expression OP_GTE expression  */
#line 552 "mini_compiler.y"
                                   { (yyval.dtype) = TYPE_INT; }
#line 1988 "mini_compiler.tab.c"
    break;

  case 81: /* expression: expression '<' expression  */
#line 553 "mini_compiler.y"
                                   { (yyval.dtype) = TYPE_INT; }
#line 1994 "mini_compiler.tab.c"
    break;

  case 82: /* expression: expression '>' expression  */
#line 554 "mini_compiler.y"
                                   { (yyval.dtype) = TYPE_INT; }
#line 2000 "mini_compiler.tab.c"
    break;

  case 83: /* expression: expression '+' expression  */
#line 556 "mini_compiler.y"
        {
            /* warn on mixing float + int without cast */
            if (((yyvsp[-2].dtype)==TYPE_FLOAT&&(yyvsp[0].dtype)==TYPE_INT)||((yyvsp[-2].dtype)==TYPE_INT&&(yyvsp[0].dtype)==TYPE_FLOAT))
                sem_warning("Mixing int and float in '+'",
                            "Consider casting: (float)x + y");
            (yyval.dtype) = ((yyvsp[-2].dtype)==TYPE_FLOAT||(yyvsp[0].dtype)==TYPE_FLOAT||
                  (yyvsp[-2].dtype)==TYPE_DOUBLE||(yyvsp[0].dtype)==TYPE_DOUBLE) ? TYPE_FLOAT : TYPE_INT;
        }
#line 2013 "mini_compiler.tab.c"
    break;

  case 84: /* expression: expression '-' expression  */
#line 564 "mini_compiler.y"
                                   { (yyval.dtype) = TYPE_INT; }
#line 2019 "mini_compiler.tab.c"
    break;

  case 85: /* expression: expression '*' expression  */
#line 565 "mini_compiler.y"
                                   { (yyval.dtype) = TYPE_INT; }
#line 2025 "mini_compiler.tab.c"
    break;

  case 86: /* expression: expression '/' expression  */
#line 567 "mini_compiler.y"
        {
            /* Warn on integer division */
            if ((yyvsp[-2].dtype)==TYPE_INT && (yyvsp[0].dtype)==TYPE_INT)
                sem_warning("Integer division may truncate result",
                            "Cast one operand to float if fractional result needed");
            (yyval.dtype) = TYPE_INT;
        }
#line 2037 "mini_compiler.tab.c"
    break;

  case 87: /* expression: expression '%' expression  */
#line 574 "mini_compiler.y"
                                   { (yyval.dtype) = TYPE_INT; }
#line 2043 "mini_compiler.tab.c"
    break;

  case 88: /* expression: expression OP_LSHIFT expression  */
#line 575 "mini_compiler.y"
                                      { (yyval.dtype) = TYPE_INT; }
#line 2049 "mini_compiler.tab.c"
    break;

  case 89: /* expression: expression OP_RSHIFT expression  */
#line 576 "mini_compiler.y"
                                      { (yyval.dtype) = TYPE_INT; }
#line 2055 "mini_compiler.tab.c"
    break;

  case 90: /* expression: expression '|' expression  */
#line 577 "mini_compiler.y"
                                { (yyval.dtype) = TYPE_INT; }
#line 2061 "mini_compiler.tab.c"
    break;

  case 91: /* expression: expression '&' expression  */
#line 578 "mini_compiler.y"
                                { (yyval.dtype) = TYPE_INT; }
#line 2067 "mini_compiler.tab.c"
    break;

  case 92: /* expression: expression '^' expression  */
#line 579 "mini_compiler.y"
                                { (yyval.dtype) = TYPE_INT; }
#line 2073 "mini_compiler.tab.c"
    break;

  case 93: /* unary_expr: primary_expr  */
#line 583 "mini_compiler.y"
                                 { (yyval.dtype) = (yyvsp[0].dtype); }
#line 2079 "mini_compiler.tab.c"
    break;

  case 94: /* unary_expr: '-' unary_expr  */
#line 584 "mini_compiler.y"
                                  { (yyval.dtype) = (yyvsp[0].dtype); }
#line 2085 "mini_compiler.tab.c"
    break;

  case 95: /* unary_expr: '!' unary_expr  */
#line 585 "mini_compiler.y"
                                  { (yyval.dtype) = TYPE_INT; }
#line 2091 "mini_compiler.tab.c"
    break;

  case 96: /* unary_expr: '~' unary_expr  */
#line 586 "mini_compiler.y"
                                  { (yyval.dtype) = TYPE_INT; }
#line 2097 "mini_compiler.tab.c"
    break;

  case 97: /* unary_expr: OP_INC IDENTIFIER  */
#line 587 "mini_compiler.y"
                                  { sym_assign((yyvsp[0].sval)); sym_use((yyvsp[0].sval)); free((yyvsp[0].sval)); (yyval.dtype) = TYPE_INT; }
#line 2103 "mini_compiler.tab.c"
    break;

  case 98: /* unary_expr: OP_DEC IDENTIFIER  */
#line 588 "mini_compiler.y"
                                  { sym_assign((yyvsp[0].sval)); sym_use((yyvsp[0].sval)); free((yyvsp[0].sval)); (yyval.dtype) = TYPE_INT; }
#line 2109 "mini_compiler.tab.c"
    break;

  case 99: /* unary_expr: IDENTIFIER OP_INC  */
#line 589 "mini_compiler.y"
                                  { sym_assign((yyvsp[-1].sval)); sym_use((yyvsp[-1].sval)); free((yyvsp[-1].sval)); (yyval.dtype) = TYPE_INT; }
#line 2115 "mini_compiler.tab.c"
    break;

  case 100: /* unary_expr: IDENTIFIER OP_DEC  */
#line 590 "mini_compiler.y"
                                  { sym_assign((yyvsp[-1].sval)); sym_use((yyvsp[-1].sval)); free((yyvsp[-1].sval)); (yyval.dtype) = TYPE_INT; }
#line 2121 "mini_compiler.tab.c"
    break;

  case 101: /* unary_expr: '&' IDENTIFIER  */
#line 591 "mini_compiler.y"
                                  { sym_use((yyvsp[0].sval)); free((yyvsp[0].sval)); (yyval.dtype) = TYPE_INT; }
#line 2127 "mini_compiler.tab.c"
    break;

  case 102: /* unary_expr: '*' IDENTIFIER  */
#line 592 "mini_compiler.y"
                                  { sym_use((yyvsp[0].sval)); free((yyvsp[0].sval)); (yyval.dtype) = TYPE_INT; }
#line 2133 "mini_compiler.tab.c"
    break;

  case 103: /* primary_expr: INT_LIT  */
#line 596 "mini_compiler.y"
                    { (yyval.dtype) = TYPE_INT;    }
#line 2139 "mini_compiler.tab.c"
    break;

  case 104: /* primary_expr: FLOAT_LIT  */
#line 597 "mini_compiler.y"
                    { (yyval.dtype) = TYPE_FLOAT;  }
#line 2145 "mini_compiler.tab.c"
    break;

  case 105: /* primary_expr: STRING_LIT  */
#line 598 "mini_compiler.y"
                    { free((yyvsp[0].sval)); (yyval.dtype) = TYPE_INT; /* char* = int-compatible */ }
#line 2151 "mini_compiler.tab.c"
    break;

  case 106: /* primary_expr: IDENTIFIER  */
#line 600 "mini_compiler.y"
        {
            DataType t = sym_use((yyvsp[0].sval));
            free((yyvsp[0].sval));
            (yyval.dtype) = t;
        }
#line 2161 "mini_compiler.tab.c"
    break;

  case 107: /* primary_expr: IDENTIFIER '(' arg_list_opt ')'  */
#line 606 "mini_compiler.y"
        {
            /* function call — look it up for its return type */
            Symbol *s = sym_find((yyvsp[-3].sval));
            DataType t = s ? s->type : TYPE_UNKNOWN;
            free((yyvsp[-3].sval));
            (yyval.dtype) = t;
        }
#line 2173 "mini_compiler.tab.c"
    break;

  case 108: /* primary_expr: IDENTIFIER '[' expression ']'  */
#line 614 "mini_compiler.y"
        {
            char arr_name[80];
            /* best-effort: just use the base name */
            DataType t = sym_use((yyvsp[-3].sval));
            free((yyvsp[-3].sval));
            (yyval.dtype) = t;
        }
#line 2185 "mini_compiler.tab.c"
    break;

  case 109: /* primary_expr: '(' expression ')'  */
#line 621 "mini_compiler.y"
                          { (yyval.dtype) = (yyvsp[-1].dtype); }
#line 2191 "mini_compiler.tab.c"
    break;

  case 110: /* primary_expr: '(' type_spec ')' unary_expr  */
#line 623 "mini_compiler.y"
        {
            /* explicit cast — warn if lossy */
            if ((yyvsp[-2].dtype)==TYPE_INT && ((yyvsp[0].dtype)==TYPE_FLOAT||(yyvsp[0].dtype)==TYPE_DOUBLE))
                sem_warning("Casting float to int truncates decimal part",
                            "Use round() or floor() if truncation is intentional");
            (yyval.dtype) = (yyvsp[-2].dtype);
        }
#line 2203 "mini_compiler.tab.c"
    break;


#line 2207 "mini_compiler.tab.c"

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

#line 642 "mini_compiler.y"


/* ============================================================
   MAIN
   ============================================================ */
int main(int argc, char *argv[]) {
    printf(BOLD CYAN
    "+----------------------------------------------------------+\n"
    "|      Educational Mini Compiler  v2.0                    |\n"
    "|  Lexical + Syntax + Semantic Analysis                   |\n"
    "+----------------------------------------------------------+\n"
    RESET "\n");

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (!f) {
            fprintf(stderr, RED "Cannot open file: %s\n" RESET, argv[1]);
            return 1;
        }
        yyin = f;
        printf(BOLD "  Analysing: %s\n\n" RESET, argv[1]);
    } else {
        printf(BOLD "  Reading stdin" RESET
               " (type code, press Ctrl+D when done):\n\n");
    }

    printf(BOLD "TOKEN STREAM:\n" RESET);
    printf("  %-10s  %-20s  %s\n", "Line", "Type", "Value");
    printf(CYAN "  --------------------------------------------------------------\n" RESET);

    yyparse();

    printf(CYAN "  --------------------------------------------------------------\n" RESET);

    /* Print remaining symbols (globals) */
    sym_check_unused_scope(0);
    if (sym_count > 0) sym_print();

    /* Summary */
    int total_errors = lex_error_count + syn_error_count + sem_error_count;
    printf(BOLD "\n  COMPILATION SUMMARY\n" RESET);
    printf("  %-32s %d\n", "Lines processed:", line_number - 1);
    printf("  %-32s " YELLOW "%d\n" RESET, "Warnings:", warning_count);
    printf("  %-32s " RED    "%d\n" RESET, "Lexical errors:",  lex_error_count);
    printf("  %-32s " RED    "%d\n" RESET, "Syntax errors:",   syn_error_count);
    printf("  %-32s " RED    "%d\n" RESET, "Semantic errors:", sem_error_count);
    printf("  %-32s " RED    "%d\n" RESET, "Total errors:",    total_errors);

    if (total_errors == 0 && warning_count == 0)
        printf(GREEN "\n  All good! No errors or warnings.\n\n" RESET);
    else if (total_errors == 0)
        printf(YELLOW "\n  Compiled with %d warning(s). Review above.\n\n" RESET, warning_count);
    else
        printf(RED "\n  Fix %d error(s) before running GCC.\n\n" RESET, total_errors);

    if (argc > 1) fclose(yyin);
    return (total_errors > 0) ? 1 : 0;
}
