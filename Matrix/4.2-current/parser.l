%option c++

%option noyywrap nounput noinput batch debug
%option yylineno
%option yyclass="Scanner"

%{

#include "parser.tab.hh"
#include "scanner.h"

#define YY_USER_ACTION updateLocation();
%}

WS      [ \t\v]+
LBREAK  [\n]+
UNOP    [\+\-]
DIGIT   [0-9]
DIGIT1  [1-9]
FDIGIT  [0-9]+\.[0-9]+

%%

{WS}              /* skip blanks and tabs */
{LBREAK}                        return yy::parser::token_type::LBREAK;
"\-\-"                          return yy::parser::token_type::EDGE;
","                             return yy::parser::token_type::COMMA;
"V"                             return yy::parser::token_type::EDSDIM;
";"                             return yy::parser::token_type::SCOLON;
{UNOP}{0,1}{FDIGIT}             return yy::parser::token_type::NUMBER;
{UNOP}{0,1}{DIGIT}*     return yy::parser::token_type::NUMBER;
.			                    return yy::parser::token_type::ERR;

%%

// nothing