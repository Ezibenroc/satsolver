%{
#include "extended_formula.y.hpp"

extern "C" {
    int yylex();
}

%}

%%
[  \t\n]            { }
[a-z][a-zA-Z0-9]*   { yylval.type_string = (char*) malloc(strlen(yytext)+1); strcpy(yylval.type_string, yytext); return VAR; }
"~"                 { return NOT; }
"("                 { return LEFT_PAREN; }
")"                 { return RIGHT_PAREN; }
"\\/"               { return OR; }
"/\\"               { return AND; }
"=>"                { return IMPLIES; }

%%
