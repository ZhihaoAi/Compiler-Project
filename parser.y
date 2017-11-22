%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include "header.h"
    #include "hashtable.h"

    /* prototypes */
    nodeType *opr(int oper, int nops, ...);
    nodeType *id(char *);
    nodeType *con(int value);
    void freeNode(nodeType *p);
    int ex(nodeType *p);
    int yylex(void);
    void yyerror(char *s);
    hashtable_t *ht;
    int sym[26];                    /* symbol table */
%}

%union {
    int intValue;               /* integer value */
    char *idstr;
    // unsigned int symbolIndex;   /* symbol table index */
    nodeType *nodePtr;             /* node pointer */
};

%token <intValue> INTEGER
%token <idstr> IDENTIFIER
%token FOR WHILE IF PRINT READ BREAK CONTINUE

/* [Operator Precedence](http://en.cppreference.com/w/c/language/operator_precedence) */
%left ','

%right '='

%left OR
%left AND

%left '|'
%left '^'
%left '&'

%left EQ NE
%left GE LE '>' '<'

%left '+' '-'
%left '*' '/' '%'
%right '!' '~' UMINUS UPLUS

%left SUBSCRIPT
%nonassoc IF_WO_ELSE
%nonassoc ELSE

%type <nodePtr> stmt expr stmt_list var

%%

program:
          lines              { exit(0); }
        ;

lines:
          lines stmt         { ex($2); freeNode($2); }
        | /* NULL */
        ;

stmt:
          ';'                             { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                        { $$ = $1; }
        | PRINT expr ';'                  { $$ = opr(PRINT, 1, $2); }
	    | READ var ';'		              { $$ = opr(READ, 1, $2); }
        | var '=' expr ';'                { $$ = opr('=', 2, $1, $3); }
        | BREAK ';'                       { $$ = opr(BREAK, 2, NULL, NULL);}
        | CONTINUE ';'                    { $$ = opr(CONTINUE, 2, NULL, NULL);}
	    | FOR '(' stmt stmt stmt ')' stmt { $$ = opr(FOR, 4, $3, $4, $5, $7); }
        | WHILE '(' expr ')' stmt         { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IF_WO_ELSE     { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt  { $$ = opr(IF, 3, $3, $5, $7); }
        | '{' stmt_list '}'               { $$ = $2; }
        ;

stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;

expr:
          INTEGER               { $$ = con($1); }
        | var                   { $$ = $1; }
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
        | '+' expr %prec UPLUS  { $$ = opr(UPLUS, 1, $2); }
        | '~' expr              { $$ = opr('~', 1, $2); }

        | expr '&' expr         { $$ = opr('&', 2, $1, $3); }
        | expr '|' expr         { $$ = opr('|', 2, $1, $3); }
        | expr '^' expr         { $$ = opr('^', 2, $1, $3); }

        | expr '+' expr         { $$ = opr('+', 2, $1, $3); }
        | expr '-' expr         { $$ = opr('-', 2, $1, $3); }
        | expr '*' expr         { $$ = opr('*', 2, $1, $3); }
        | expr '/' expr         { $$ = opr('/', 2, $1, $3); }
        | expr '%' expr         { $$ = opr('%', 2, $1, $3); }

        | expr '<' expr         { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr         { $$ = opr('>', 2, $1, $3); }
        | expr GE expr          { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr          { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr          { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr          { $$ = opr(EQ, 2, $1, $3); }

        | '!' expr              { $$ = opr('!', 1, $2); }
	    | expr AND expr		    { $$ = opr(AND, 2, $1, $3); }
	    | expr OR expr		    { $$ = opr(OR, 2, $1, $3); }
        | '(' expr ')'          { $$ = $2; }
        ;

var:
          IDENTIFIER                { $$ = id($1); }
        | IDENTIFIER '[' expr ']'   { $$ = opr(SUBSCRIPT, 2, id($1), $3); }
        ;
%%

#define SIZEOF_NODEENUM ((char *)&p->con - (char *)p)

nodeType *con(int value) {
    nodeType *p;
    size_t nodeSize;

    nodeSize = SIZEOF_NODEENUM + sizeof(conNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("Out of memory");

    p->type = typeCon;
    p->con.value = value;

    return p;
}

nodeType *id(char *key) {
    nodeType *p;
    size_t nodeSize;

    nodeSize = SIZEOF_NODEENUM 
                + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("Out of memory");

    p->type = typeId;
    sprintf(p->id.key, "%s", key);
    // printf("%s\n", p->id.key);
    // p->id.key = key;

    return p;
}

nodeType *opr(int oper, int nops, ...) {
    nodeType *p;
    size_t nodeSize;
    int i;

    nodeSize = SIZEOF_NODEENUM 
                + sizeof(oprNodeType) 
                + (nops) * sizeof(nodeType*);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("Out of memory");

    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;

    va_list ap;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free (p);
}

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}

int main(int argc, char **argv) {
    extern FILE* yyin;
    if (argc > 1){
        FILE* in = fopen(argv[1], "r");
        if (in == NULL){
            char* def_test = "default_test.mp";
            printf("Failed openning \"%s\". Using %s instead.\n", argv[1], def_test);
            yyin = fopen(def_test, "r");
        } else {
            yyin = in;
        }
    } else {
        yyin = stdin;
    }
    ht = make_hashtable(100);
    yyparse();
    return 0;
}
