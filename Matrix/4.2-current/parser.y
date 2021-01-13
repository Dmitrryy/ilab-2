%language "c++"

%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%param {yy::ParsDriver* driver}

%code requires
{
#include <algorithm>
#include <string>
#include <vector>
#include <map>

// forward decl of argument to parser
namespace yy { class ParsDriver; }
}

%code
{
#include "ParserDriver.h"
#include <optional>

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval,
                         ParsDriver* driver);
}
}

%token
  LBREAK  "\n"
  TEOF
  EDGE    "--"
  COMMA   ","
  SCOLON  ";"
  EDSDIM  "V"
  ERR
;

%token < float > NUMBER
%nterm < float > vertex
%nterm < float > resister
%nterm < float > voltage
%nterm eds_dimention

%left '+' '-'

%start program

%%

program
    : vertex EDGE vertex COMMA resister SCOLON voltage program { 	ezg::Edge cur($1, $3);
    									cur.resistance = $5;
    									cur.eds = $7;
    									driver->insert(cur);
     }
    | {}
;

vertex
    : NUMBER { $$ = $1; }
;

resister
    : NUMBER { $$ = $1; }
;

voltage
    : NUMBER eds_dimention LBREAK 	{ $$ = $1; }
    | NUMBER eds_dimention 		{ $$ = $1; }
    | LBREAK        			{ $$ = 0.f; }
    | /*empty*/     			{ $$ = 0.f; }
;

eds_dimention
:	EDSDIM {}
| 	/* empty */ {}
;

%%

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval,
                         ParsDriver* driver)
{
  return driver->yylex(yylval);
}

void parser::error(const std::string& msg){
    std::cerr << msg << std::endl;
}
}