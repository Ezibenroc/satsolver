%{
#include "congruence.y.hpp"

extern "C" {
    int yylex();
}

%}

%%
[  \t\n]            { }
[1-9][0-9]*         { yylval.type_integer = atoi(yytext); return INT; }
x[0-9][0-9]*        { yylval.type_integer = atoi(yytext+1); return ID; }
[a-wyzA-Z][a-zA-Z0-9]*    { yylval.type_string = (char*) malloc(strlen(yytext)+1); strcpy(yylval.type_string, yytext); return F_SYMBOL; }
"~"                 { return NOT; }
"("                 { return LEFT_PAREN; }
")"                 { return RIGHT_PAREN; }
"\\/"               { return OR; }
"/\\"               { return AND; }
"=>"                { return IMPLIES; }

%{ /* For theories */ %}
","                 { return COMMA; }
"="                 { return EQUAL; }
"!="                { return UNEQUAL; }

%%

