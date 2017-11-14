%{
#include <stdlib.h>
#include "header.h"
#include "parser.tab.h"
void yyerror(char *);
%}

%%

[a-z]       { 
                yylval.symbolIndex = *yytext - 'a';
                return VARIABLE;
            }

[0-9]+      {
                yylval.intValue = atoi(yytext);
                return INTEGER;
            }

[-()<\[\]>=*+/;{}.%] {
                return *yytext;
             }

">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"for"		    return FOR;
"while"         return WHILE;
"if"            return IF;
"else"          return ELSE;
"print"         return PRINT;
"break"         return BREAK;
"continue"      return CONTINUE;
"read"		    return READ;
"&&"		    return AND;
"||"		    return OR;

\/\/.*\n	;	/* skip comments */

[ \t\n]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}
