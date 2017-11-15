%option noyywrap
%{
#include <stdlib.h>
#include "header.h"
#include "parser.tab.h"
extern void yyerror(char *);
%}

ic      "//"[^\n]*
bc      "/*"([^*]|(\*[^/]))*"*/"
alpha	[[:alpha:]]
digit	[[:digit:]]
hexdigit[a-fA-F0-9]
alnum   [[:alnum:]]
id 		{alpha}

%%

{id}		{ yylval.symbolIndex = *yytext - 'a'; return IDENTIFIER; }

-?0[xX]{hexdigit}+	{ yylval.intValue = strtol(yytext, NULL, 16); return(INTEGER); }
-?0{digit}+			{ yylval.intValue = strtol(yytext, NULL, 8); return(INTEGER); }
-?{digit}+			{ yylval.intValue = atoi(yytext); return INTEGER; }

"("				return '(';
")"				return ')';
"["				return '[';
"]"				return ']';
"{"				return '{';
"}"				return '}';
"."				return '.';
";"				return ';';

"&&"		    return AND;
"||"		    return OR;
"!"				return '!';

"&"				return '&';
"|"				return '|';
"^"				return '^';
"~"				return '~';

"+"				return '+';
"-"				return '-';
"*"				return '*';
"/"				return '/';
"%"				return '%';

"<"				return '<';
">"				return '>';
"=" 			return '=';
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

"true"|"True"|"TRUE"		{ yylval.intValue = 1; return INTEGER; }
"false"|"False"|"FALSE"		{ yylval.intValue = 0; return INTEGER; }

{ic}|{bc}	;	/* skip comments */

[ \t\n]+	;	/* ignore whitespace */

.               yyerror("Unknown character");
%%
