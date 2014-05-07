%{
#include "difference.y.hpp"

extern "C" {
    int yylex();
}

%}

%%
[  \t\n]            { }
-?(0|[1-9][0-9]*)     { yylval.type_integer = atoi(yytext); return INT; }
x(0|[1-9][0-9]*)   { yylval.type_string = (char*) malloc(strlen(yytext)+1); strcpy(yylval.type_string, yytext); return VAR; }
"~"                 { return NOT; }
"("                 { return LEFT_PAREN; }
")"                 { return RIGHT_PAREN; }
"\\/"               { return OR; }
"/\\"               { return AND; }
"=>"                { return IMPLIES; }
"-"                 { return MINUS; }
"<"                 { return LOWER; }
">"                 { return GREATER; }
"<="                { return LEQ; }
">="                { return GEQ; }
"="                 { return EQUAL; }
"!="                { return UNEQUAL; }

%%

