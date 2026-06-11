/* ============================================================
   Educational Mini Compiler  v2.0
   Phase 2 : Syntax Analysis  (bison)
   Phase 3 : Semantic Analysis (symbol table + type checking)
   ============================================================ */

%{
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
%}

/* ---- value types ---- */
%union {
    int      ival;
    double   fval;
    char    *sval;
    int      dtype;   /* DataType enum value */
}

/* ---- token declarations ---- */
%token KW_INT KW_FLOAT KW_CHAR KW_DOUBLE KW_LONG KW_SHORT KW_VOID
%token KW_IF KW_ELSE KW_WHILE KW_FOR KW_DO
%token KW_RETURN KW_BREAK KW_CONTINUE KW_STRUCT
%token DIRECTIVE
%token OP_EQ OP_NEQ OP_LTE OP_GTE OP_INC OP_DEC
%token OP_PLUS_EQ OP_MINUS_EQ OP_MULT_EQ OP_DIV_EQ
%token OP_AND OP_OR OP_RSHIFT OP_LSHIFT ARROW

%token <sval> IDENTIFIER STRING_LIT
%token <ival> INT_LIT
%token <fval> FLOAT_LIT

%type  <dtype> type_spec expression unary_expr primary_expr
%type  <sval>  declarator

/* ---- precedence (low → high) ---- */
%right '='  OP_PLUS_EQ OP_MINUS_EQ OP_MULT_EQ OP_DIV_EQ
%left  OP_OR
%left  OP_AND
%left  '|'
%left  '^'
%left  '&'
%left  OP_EQ OP_NEQ
%left  '<' '>' OP_LTE OP_GTE
%left  OP_LSHIFT OP_RSHIFT
%left  '+' '-'
%left  '*' '/' '%'
%right '!' '~' UMINUS
%left  OP_INC OP_DEC '.' ARROW '[' '('

%%

/* ============================================================
   GRAMMAR
   ============================================================ */

program
    : /* empty */
    | program top_level_item
    ;

top_level_item
    : directive
    | function_def
    | declaration ';'
    | error ';'  { yyerrok; }
    ;

/* ---- preprocessor directives (pass through) ---- */
directive
    : DIRECTIVE IDENTIFIER           { free($2); }
    | DIRECTIVE '<' IDENTIFIER '>'   { free($3); }
    | DIRECTIVE STRING_LIT           { free($2); }
    ;

/* ============================================================
   FUNCTION DEFINITION
   ============================================================ */
 function_def
    : type_spec IDENTIFIER '(' param_list_opt ')'
        {
            sym_declare($2,
                        (DataType)$1,
                        SYMBOL_FUNCTION);

            Symbol *s = sym_find($2);

            if (s)
                s->initialized = 1;

            free($2);

            cur_scope++;
        }
      compound_stmt
        {
            sym_pop_scope(cur_scope);
            cur_scope--;
        }
    ;

param_list_opt
    : /* empty */
    | param_list
    ;

param_list
    : param_decl
    | param_list ',' param_decl
    ;

param_decl
    : type_spec IDENTIFIER
        {
            sym_declare($2,
                        (DataType)$1,
                        SYMBOL_VARIABLE);

            /* parameters are pre-initialised by caller */
            Symbol *s = sym_find($2);

            if (s)
                s->initialized = 1;

            free($2);
        }
    | type_spec
    ;

/* ============================================================
   STATEMENTS
   ============================================================ */
compound_stmt
    : '{'
        { cur_scope++; }
      stmt_list
      '}'
        {
            sym_pop_scope(cur_scope);
            cur_scope--;
        }
    ;

stmt_list
    : /* empty */
    | stmt_list statement
    ;

statement
    : declaration ';'
    | expression_stmt
    | compound_stmt
    | if_stmt
    | while_stmt
    | for_stmt
    | do_while_stmt
    | return_stmt
    | break_stmt
    | continue_stmt
    | ';'
    | error ';'   { yyerrok; }
    ;

/* ---- declaration ---- */
declaration
    : type_spec declarator_list
    ;

declarator_list
    : declarator_item
    | declarator_list ',' declarator_item
    ;

declarator_item
    : declarator
        {sym_declare($1,
            cur_decl_type,
            SYMBOL_VARIABLE); }
    | declarator '=' expression
        {
            sym_declare($1,
            cur_decl_type,
            SYMBOL_VARIABLE);

            sym_assign($1);
            sym_assign($1);
            /* type check: cur_decl_type vs expression type ($3) */
            if (!types_compatible(cur_decl_type, (DataType)$3)) {
                char msg[80];
                snprintf(msg, sizeof(msg),
                         "Type mismatch: assigning '%s' to '%s %s'",
                         type_name((DataType)$3),
                         type_name(cur_decl_type), $1);
                sem_error(msg, "Types are incompatible",
                          "Use a cast or change the variable type");
            }
            free($1);
        }
    | declarator '[' INT_LIT ']'
        {
            char arr_name[80];
            snprintf(arr_name, sizeof(arr_name), "%s[%d]", $1, $3);
            sym_declare(arr_name,
            cur_decl_type,
            SYMBOL_VARIABLE);
            free($1);
        }
    ;

declarator
    : IDENTIFIER  { $$ = $1; }
    | '*' IDENTIFIER { /* pointer */ $$ = $2; }
    ;

type_spec
    : KW_INT    { cur_decl_type = TYPE_INT;    $$ = TYPE_INT;    }
    | KW_FLOAT  { cur_decl_type = TYPE_FLOAT;  $$ = TYPE_FLOAT;  }
    | KW_CHAR   { cur_decl_type = TYPE_CHAR;   $$ = TYPE_CHAR;   }
    | KW_DOUBLE { cur_decl_type = TYPE_DOUBLE; $$ = TYPE_DOUBLE; }
    | KW_LONG   { cur_decl_type = TYPE_LONG;   $$ = TYPE_LONG;   }
    | KW_SHORT  { cur_decl_type = TYPE_SHORT;  $$ = TYPE_SHORT;  }
    | KW_VOID   { cur_decl_type = TYPE_VOID;   $$ = TYPE_VOID;   }
    ;

/* ---- expression statement ---- */
expression_stmt
    : expression ';'
    | expression error  { yyerrok;
                          sem_error("Missing semicolon",
                                    "Every C statement must end with ';'",
                                    "Add ';' at the end of this statement"); }
    ;

/* ---- if ---- */
if_stmt
    : KW_IF '(' expression ')' statement
    | KW_IF '(' expression ')' statement KW_ELSE statement
    | KW_IF error ')' statement   { yyerrok; }
    ;

/* ---- while ---- */
while_stmt
    : KW_WHILE '(' expression ')' statement
    | KW_WHILE error ')' statement { yyerrok; }
    ;

/* ---- for ---- */
for_stmt
    : KW_FOR '(' for_init expression ';' expression ')' statement
    | KW_FOR '(' for_init ';' expression ')' statement
    | KW_FOR '(' for_init expression ')' statement
    | KW_FOR error ')' statement { yyerrok; }
    ;

for_init
    : ';'
    | expression ';'
    | declaration ';'
    ;

/* ---- do-while ---- */
do_while_stmt
    : KW_DO statement KW_WHILE '(' expression ')' ';'
    ;

/* ---- return ---- */
return_stmt
    : KW_RETURN expression ';'
    | KW_RETURN ';'
    ;

break_stmt    : KW_BREAK    ';' ;
continue_stmt : KW_CONTINUE ';' ;

/* ============================================================
   EXPRESSIONS  (return DataType)
   ============================================================ */
expression
    : unary_expr
    | IDENTIFIER '=' expression
        {
            sym_assign($1);
            Symbol *s = sym_find($1);
            DataType ltype = s ? s->type : TYPE_UNKNOWN;
            if (!types_compatible(ltype, (DataType)$3)) {
                char msg[80];
                snprintf(msg, sizeof(msg),
                         "Type mismatch: assigning '%s' to '%s'",
                         type_name((DataType)$3), type_name(ltype));
                sem_error(msg, "Incompatible types in assignment",
                          "Use explicit cast or change variable type");
            }
            free($1);
            $$ = ltype;
        }
    | IDENTIFIER OP_PLUS_EQ  expression { sym_assign($1); free($1); $$ = TYPE_INT; }
    | IDENTIFIER OP_MINUS_EQ expression { sym_assign($1); free($1); $$ = TYPE_INT; }
    | IDENTIFIER OP_MULT_EQ  expression { sym_assign($1); free($1); $$ = TYPE_INT; }
    | IDENTIFIER OP_DIV_EQ   expression { sym_assign($1); free($1); $$ = TYPE_INT; }
    | expression OP_AND expression { $$ = TYPE_INT; }
    | expression OP_OR  expression { $$ = TYPE_INT; }
    | expression OP_EQ  expression { $$ = TYPE_INT; }
    | expression OP_NEQ expression { $$ = TYPE_INT; }
    | expression OP_LTE expression { $$ = TYPE_INT; }
    | expression OP_GTE expression { $$ = TYPE_INT; }
    | expression '<'    expression { $$ = TYPE_INT; }
    | expression '>'    expression { $$ = TYPE_INT; }
    | expression '+'    expression
        {
            /* warn on mixing float + int without cast */
            if (($1==TYPE_FLOAT&&$3==TYPE_INT)||($1==TYPE_INT&&$3==TYPE_FLOAT))
                sem_warning("Mixing int and float in '+'",
                            "Consider casting: (float)x + y");
            $$ = ($1==TYPE_FLOAT||$3==TYPE_FLOAT||
                  $1==TYPE_DOUBLE||$3==TYPE_DOUBLE) ? TYPE_FLOAT : TYPE_INT;
        }
    | expression '-'    expression { $$ = TYPE_INT; }
    | expression '*'    expression { $$ = TYPE_INT; }
    | expression '/'    expression
        {
            /* Warn on integer division */
            if ($1==TYPE_INT && $3==TYPE_INT)
                sem_warning("Integer division may truncate result",
                            "Cast one operand to float if fractional result needed");
            $$ = TYPE_INT;
        }
    | expression '%'    expression { $$ = TYPE_INT; }
    | expression OP_LSHIFT expression { $$ = TYPE_INT; }
    | expression OP_RSHIFT expression { $$ = TYPE_INT; }
    | expression '|' expression { $$ = TYPE_INT; }
    | expression '&' expression { $$ = TYPE_INT; }
    | expression '^' expression { $$ = TYPE_INT; }
    ;

unary_expr
    : primary_expr               { $$ = $1; }
    | '-' unary_expr %prec UMINUS { $$ = $2; }
    | '!' unary_expr              { $$ = TYPE_INT; }
    | '~' unary_expr              { $$ = TYPE_INT; }
    | OP_INC IDENTIFIER           { sym_assign($2); sym_use($2); free($2); $$ = TYPE_INT; }
    | OP_DEC IDENTIFIER           { sym_assign($2); sym_use($2); free($2); $$ = TYPE_INT; }
    | IDENTIFIER OP_INC           { sym_assign($1); sym_use($1); free($1); $$ = TYPE_INT; }
    | IDENTIFIER OP_DEC           { sym_assign($1); sym_use($1); free($1); $$ = TYPE_INT; }
    | '&' IDENTIFIER              { sym_use($2); free($2); $$ = TYPE_INT; }
    | '*' IDENTIFIER              { sym_use($2); free($2); $$ = TYPE_INT; }
    ;

primary_expr
    : INT_LIT       { $$ = TYPE_INT;    }
    | FLOAT_LIT     { $$ = TYPE_FLOAT;  }
    | STRING_LIT    { free($1); $$ = TYPE_INT; /* char* = int-compatible */ }
    | IDENTIFIER
        {
            DataType t = sym_use($1);
            free($1);
            $$ = t;
        }
    | IDENTIFIER '(' arg_list_opt ')'
        {
            /* function call — look it up for its return type */
            Symbol *s = sym_find($1);
            DataType t = s ? s->type : TYPE_UNKNOWN;
            free($1);
            $$ = t;
        }
    | IDENTIFIER '[' expression ']'
        {
            char arr_name[80];
            /* best-effort: just use the base name */
            DataType t = sym_use($1);
            free($1);
            $$ = t;
        }
    | '(' expression ')'  { $$ = $2; }
    | '(' type_spec ')' unary_expr
        {
            /* explicit cast — warn if lossy */
            if ($2==TYPE_INT && ($4==TYPE_FLOAT||$4==TYPE_DOUBLE))
                sem_warning("Casting float to int truncates decimal part",
                            "Use round() or floor() if truncation is intentional");
            $$ = $2;
        }
    ;

arg_list_opt
    : /* empty */
    | arg_list
    ;

arg_list
    : expression
    | arg_list ',' expression
    ;

%%

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
