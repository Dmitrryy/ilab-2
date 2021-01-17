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
%nterm < size_t > vertex
%nterm < float > resister
%nterm < float > voltage
%nterm expr
%nterm line
%nterm eds_dimention
%nterm end_file


%start program

%%

program
: line	program	{	}
| /* empty */ 	{ 	}
;


line
:	expr LBREAK 	{	}
|	expr end_file 	{ 	}
|	LBREAK		{	}
| 	error LBREAK 	{  	}
| 	error TEOF 	{  	}
;


expr
: vertex EDGE vertex COMMA resister SCOLON voltage 	{	driver->connect($1, $3, $5, $7);	}
;


end_file
:	TEOF end_file 	{	}
| 	/* empty */ 	{	}
;


vertex
    : NUMBER { $$ = static_cast< size_t >($1); }
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