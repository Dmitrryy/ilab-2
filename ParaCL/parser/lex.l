%option c++
/* TODO */
%option noyywrap nounput noinput batch debug
%option yylineno
%option yyclass="Scanner"

%{

#include "gramar.tab.hh"
#include "scanner.h"
#include <iostream>

#define YY_USER_ACTION updateLocation();

%}


WS              [ \t\v]+
ENDL            [\n]+
UNOP            [\-\+]
DOT             [.]
DIGIT           [0-9]
DIGIT_NON_ZERO  [1-9]
DOUBLE          {DIGIT}+{DOT}?{DIGIT}*
INTEGER         {DIGIT_NON_ZERO}{DIGIT}*
LETTER          [a-zA-Z_]
LETTER_OR_DIGIT [a-zA-Z0-9_]
VARIABLE        {LETTER}{LETTER_OR_DIGIT}*

COMMENT1        "//".*

%%
                            /* Сопоставляем каждой лексеме её константу для парсера */
{WS}                        { /* DO NOTHING */  }
{ENDL}                      { /* DO NOTHING */  }
{COMMENT1}                  { /* DO NOTHING */  }

"+"                         return yy::parser::token_type::PLUS;
"-"                         return yy::parser::token_type::MINUS;
"*"                         return yy::parser::token_type::MUL;
"/"                         return yy::parser::token_type::DIV;

">"                         return yy::parser::token_type::GREATER;
"<"                         return yy::parser::token_type::LESS;
"<="                         return yy::parser::token_type::LLESS;
">="                         return yy::parser::token_type::LGREATER;
"=="                        return yy::parser::token_type::EQUAL;
"!="                        return yy::parser::token_type::NONEQUAL;

";"                         return yy::parser::token_type::SCOLON;
"("                         return yy::parser::token_type::LPARENTHESES;
")"                         return yy::parser::token_type::RPARENTHESES;
"{"                         return yy::parser::token_type::LBRACE;
"}"                         return yy::parser::token_type::RBRACE;

"?"                         return yy::parser::token_type::QMARK;
"="                         return yy::parser::token_type::ASSIGN;
"while"                     return yy::parser::token_type::KW_WHILE;
"if"                        return yy::parser::token_type::KW_IF;
"print"                     return yy::parser::token_type::KW_PRINT;
"func"                      return yy::parser::token_type::KW_FUNC;
"return"                    return yy::parser::token_type::KW_RETURN;

{INTEGER}                   return yy::parser::token_type::NUMBER;
{DIGIT}                     return yy::parser::token_type::NUMBER;
{VARIABLE}                  return yy::parser::token_type::VARIABLE;
.			                return yy::parser::token_type::ERROR;

%%