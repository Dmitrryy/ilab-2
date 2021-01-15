%language "c++"

%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%define parse.error custom

%param {ParsDriver* driver}

%locations

%code requires
{
#include <algorithm>
#include <string>
#include <vector>
#include <map>

#include <circuit/Edge.h>
// forward decl of argument to parser
namespace yy { class ParsDriver; }

}

%code
{
#include "ParserDriver.h"
#include <optional>

	namespace yy {

		parser::token_type yylex (parser::semantic_type* yylval, parser::location_type* l, ParsDriver* driver);

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
%nterm < ezg::Edge > expr
%nterm < ezg::Edge > line
%nterm eds_dimention
%nterm end_file


%start program

%%

program
: line	program	{ driver->insert($1); 	}
| /* empty */ 	{ 			}
;


line
:	expr LBREAK 	{ $$ = $1;	}
|	expr end_file 	{ $$ = $1;	}
| 	error LBREAK 	{  }
| 	error TEOF 	{  }
;


expr
: vertex EDGE vertex COMMA resister SCOLON voltage 	{	$$ = ezg::Edge($1, $3);
								$$.resistance = $5;
								$$.eds = $7;
}
;


end_file
:	TEOF end_file 	{	}
| 	/* empty */ 	{	}
;


vertex
    : NUMBER { $$ = $1; }
;

resister
    : NUMBER { $$ = $1; }
;

voltage
    : NUMBER eds_dimention		{ $$ = $1; }
    | /*empty*/     			{ $$ = 0.f; }
;

eds_dimention
:	EDSDIM 		{	}
| 	/* empty */ 	{	}
;

%%

namespace yy {

	parser::token_type yylex (parser::semantic_type* yylval, parser::location_type* l, ParsDriver* driver) {
		return driver->yylex (l, yylval);
	}

	void parser::error (const parser::location_type& l, const std::string& msg) {
		std::cout << msg << " in line: " << l.begin.line << std::endl;
	}

	void parser::report_syntax_error(parser::context const& ctx) const
	{
		driver->error(ctx);
	}

}