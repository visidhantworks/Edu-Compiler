/* ============================================================
   Educational Mini Compiler  v2.0
   Phase 1 : Lexical Analysis  (flex)
   - Feeds tokens to the Bison parser
   - Keeps ANSI colour token-stream output
   - Reports lex-level errors (bad chars, unclosed strings, etc.)
   ============================================================ */

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mini_compiler.tab.h"   /* Bison-generated token codes */

/* ANSI colours */
#define RED    "\033[1;31m"
#define YELLOW "\033[1;33m"
#define GREEN  "\033[1;32m"
#define CYAN   "\033[1;36m"
#define BLUE   "\033[1;34m"
#define RESET  "\033[0m"
#define BOLD   "\033[1m"

/* Shared with parser */
extern int line_number;
extern int lex_error_count;
extern int warning_count;

/* --------------------------------------------------------
   Token-stream pretty-printer
   -------------------------------------------------------- */
void print_token(const char *type, const char *value) {
    const char *col = RESET;
    if      (strncmp(type, "KEYWORD",  7) == 0) col = BLUE;
    else if (strncmp(type, "INTEGER",  7) == 0) col = GREEN;
    else if (strncmp(type, "FLOAT",    5) == 0) col = GREEN;
    else if (strncmp(type, "STRING",   6) == 0) col = GREEN;
    else if (strncmp(type, "IDENT",    5) == 0) col = CYAN;
    else if (strncmp(type, "OP_",      3) == 0) col = YELLOW;

    printf("  " BOLD "[Line %2d]" RESET "  %s%-18s" RESET "  \"%s\"\n",
           line_number, col, type, value);
}

/* --------------------------------------------------------
   Lex-level error / warning reporters
   -------------------------------------------------------- */
void lex_error(const char *what, const char *why, const char *fix) {
    char w1[52], w2[52], w3[52];
    snprintf(w1, sizeof(w1), "%-50s", what);
    snprintf(w2, sizeof(w2), "%-50s", why);
    snprintf(w3, sizeof(w3), "%-50s", fix);
    printf(RED
    "\n  +----------------------------------------------------+\n"
    "  |  LEX ERROR at Line %-3d                           |\n"
    "  +----------------------------------------------------+\n"
    "  |  What  : %s|\n"
    "  |  Why   : %s|\n"
    "  |  Fix   : %s|\n"
    "  +----------------------------------------------------+\n\n"
    RESET, line_number, w1, w2, w3);
    lex_error_count++;
}

void lex_warning(const char *what, const char *tip) {
    char w1[52], w2[52];
    snprintf(w1, sizeof(w1), "%-50s", what);
    snprintf(w2, sizeof(w2), "%-50s", tip);
    printf(YELLOW
    "\n  +----------------------------------------------------+\n"
    "  |  WARNING at Line %-3d                             |\n"
    "  +----------------------------------------------------+\n"
    "  |  %s|\n"
    "  |  Tip: %s|\n"
    "  +----------------------------------------------------+\n\n"
    RESET, line_number, w1, w2);
    warning_count++;
}
%}

%option noyywrap noinput nounput

DIGIT       [0-9]
LETTER      [a-zA-Z_]
INTEGER     {DIGIT}+
FLOAT       {DIGIT}+"."{DIGIT}+
IDENTIFIER  {LETTER}({LETTER}|{DIGIT})*

%%

    /* ---- COMMENTS ---- */
"//"[^\n]*              { /* single-line comment: skip */ }
"/*"([^*]|\*+[^*/])*\*+"/" {
    int i;
    for (i = 0; yytext[i]; i++)
        if (yytext[i] == '\n') line_number++;
}

    /* ---- KEYWORDS ---- */
"int"      { print_token("KEYWORD", yytext); return KW_INT;    }
"float"    { print_token("KEYWORD", yytext); return KW_FLOAT;  }
"char"     { print_token("KEYWORD", yytext); return KW_CHAR;   }
"double"   { print_token("KEYWORD", yytext); return KW_DOUBLE; }
"long"     { print_token("KEYWORD", yytext); return KW_LONG;   }
"short"    { print_token("KEYWORD", yytext); return KW_SHORT;  }
"void"     { print_token("KEYWORD", yytext); return KW_VOID;   }
"if"       { print_token("KEYWORD", yytext); return KW_IF;     }
"else"     { print_token("KEYWORD", yytext); return KW_ELSE;   }
"while"    { print_token("KEYWORD", yytext); return KW_WHILE;  }
"for"      { print_token("KEYWORD", yytext); return KW_FOR;    }
"do"       { print_token("KEYWORD", yytext); return KW_DO;     }
"return"   { print_token("KEYWORD", yytext); return KW_RETURN; }
"break"    { print_token("KEYWORD", yytext); return KW_BREAK;  }
"continue" { print_token("KEYWORD", yytext); return KW_CONTINUE;}
"struct"   { print_token("KEYWORD", yytext); return KW_STRUCT; }

"printf"   { print_token("BUILTIN",   yytext); yylval.sval = strdup(yytext); return IDENTIFIER; }
"scanf"    { print_token("BUILTIN",   yytext); yylval.sval = strdup(yytext); return IDENTIFIER; }
"main"     { print_token("BUILTIN",   yytext); yylval.sval = strdup(yytext); return IDENTIFIER; }
"#include" { print_token("DIRECTIVE", yytext); return DIRECTIVE; }
"#define"  { print_token("DIRECTIVE", yytext); return DIRECTIVE; }

    /* ---- LITERALS ---- */
{FLOAT}   {
    print_token("FLOAT", yytext);
    yylval.fval = atof(yytext);
    return FLOAT_LIT;
}
{INTEGER} {
    print_token("INTEGER", yytext);
    yylval.ival = atoi(yytext);
    return INT_LIT;
}

\"[^\"\n]*\" {
    print_token("STRING", yytext);
    yylval.sval = strdup(yytext);
    return STRING_LIT;
}
\"[^\"\n]*\n {
    lex_error("Unclosed string literal",
              "String must begin AND end with '\"'",
              "Add closing quote: \"hello\" not \"hello");
    line_number++;
}

\'[^\'\n]\'   { print_token("CHAR_LIT", yytext); yylval.ival = yytext[1]; return INT_LIT; }
\'\\[nrt\\]\' { print_token("CHAR_LIT", yytext); yylval.ival = yytext[2]; return INT_LIT; }

    /* ---- IDENTIFIERS ---- */
{IDENTIFIER} {
    if (strlen(yytext) == 1) {
        char c = yytext[0];
        if (c!='i' && c!='j' && c!='k' && c!='n' && c!='x' && c!='y') {
            char wmsg[80];
            snprintf(wmsg, sizeof(wmsg),
                     "Single-letter name '%s' is hard to read", yytext);
            lex_warning(wmsg, "Use a descriptive name: 'count', 'total', 'index'");
        }
    }
    print_token("IDENTIFIER", yytext);
    yylval.sval = strdup(yytext);
    return IDENTIFIER;
}

    /* ---- OPERATORS (multi-char first) ---- */
"=="  { print_token("OP_EQUAL",    yytext); return OP_EQ;       }
"!="  { print_token("OP_NOTEQUAL", yytext); return OP_NEQ;      }
"<="  { print_token("OP_LTE",      yytext); return OP_LTE;      }
">="  { print_token("OP_GTE",      yytext); return OP_GTE;      }
"++"  { print_token("OP_INC",      yytext); return OP_INC;      }
"--"  { print_token("OP_DEC",      yytext); return OP_DEC;      }
"+="  { print_token("OP_PLUS_EQ",  yytext); return OP_PLUS_EQ;  }
"-="  { print_token("OP_MINUS_EQ", yytext); return OP_MINUS_EQ; }
"*="  { print_token("OP_MULT_EQ",  yytext); return OP_MULT_EQ;  }
"/="  { print_token("OP_DIV_EQ",   yytext); return OP_DIV_EQ;   }
"&&"  { print_token("OP_AND",      yytext); return OP_AND;      }
"||"  { print_token("OP_OR",       yytext); return OP_OR;       }
">>"  { print_token("OP_RSHIFT",   yytext); return OP_RSHIFT;   }
"<<"  { print_token("OP_LSHIFT",   yytext); return OP_LSHIFT;   }
"->"  { print_token("ARROW",       yytext); return ARROW;       }
"="   { print_token("OP_ASSIGN",   yytext); return '=';         }
"+"   { print_token("OP_PLUS",     yytext); return '+';         }
"-"   { print_token("OP_MINUS",    yytext); return '-';         }
"*"   { print_token("OP_MULT",     yytext); return '*';         }
"/"   { print_token("OP_DIV",      yytext); return '/';         }
"%"   { print_token("OP_MOD",      yytext); return '%';         }
"<"   { print_token("OP_LT",       yytext); return '<';         }
">"   { print_token("OP_GT",       yytext); return '>';         }
"!"   { print_token("OP_NOT",      yytext); return '!';         }
"&"   { print_token("OP_BIT_AND",  yytext); return '&';         }
"|"   { print_token("OP_BIT_OR",   yytext); return '|';         }
"^"   { print_token("OP_BIT_XOR",  yytext); return '^';         }
"~"   { print_token("OP_BIT_NOT",  yytext); return '~';         }

    /* ---- PUNCTUATION ---- */
";"  { print_token("SEMICOLON", yytext); return ';'; }
","  { print_token("COMMA",     yytext); return ','; }
"."  { print_token("DOT",       yytext); return '.'; }
":"  { print_token("COLON",     yytext); return ':'; }
"["  { print_token("LBRACKET",  yytext); return '['; }
"]"  { print_token("RBRACKET",  yytext); return ']'; }
"("  { print_token("LPAREN",    yytext); return '('; }
")"  { print_token("RPAREN",    yytext); return ')'; }
"{"  { print_token("LBRACE",    yytext); return '{'; }
"}"  { print_token("RBRACE",    yytext); return '}'; }

    /* ---- WHITESPACE ---- */
\n          { line_number++; }
[ \t\r]+    { /* skip */ }

    /* ---- INVALID TOKENS ---- */
{DIGIT}+{LETTER}({LETTER}|{DIGIT})* {
    char msg[80], fix[80];
    snprintf(msg, sizeof(msg), "Invalid name '%s' starts with digit", yytext);
    snprintf(fix, sizeof(fix), "Rename to e.g. var_%s", yytext);
    lex_error(msg, "Names MUST start with letter or underscore", fix);
}
"@" { lex_error("'@' is not valid C",
                 "C does not use '@'",
                 "Remove '@' from your code"); }
"$" { lex_error("'$' is not valid C",
                 "C does not use '$'",
                 "Remove '$' and use a plain variable name"); }
"`" { lex_error("Backtick is not valid C",
                 "Backtick is a shell operator",
                 "Remove the backtick character"); }
"\\" { lex_error("Lone backslash outside string",
                  "Backslash only works inside strings",
                  "Put it inside a string: printf(\"hello\\n\");"); }
.   {
    char msg[80];
    snprintf(msg, sizeof(msg), "Unknown character '%s' (ASCII %d)",
             yytext, (unsigned char)yytext[0]);
    lex_error(msg, "This character has no meaning in C", "Delete or replace it");
}

%%
