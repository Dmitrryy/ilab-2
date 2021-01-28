%language "c++"

%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%define parse.error custom

%param {Driver* driver}

%locations
%debug


%code requires
{

	#include <algorithm>
	#include <string>
	#include <vector>

	// forward declaration of argument to parser
	namespace yy { class Driver; }

    namespace ezg {
        class INode;
        class IScope;
    }
}

%code
{

    #include "driver.h"
    #include <stack>
    #include <cassert>

    namespace ezg {
        class INode;
        class IScope;
    }

	namespace yy {

		parser::token_type yylex (parser::semantic_type* yylval, parser::location_type* l, Driver* driver);

	}

	std::stack< ezg::IScope* > gScopeStack;

}

%token
  ADD 			"+"
  SUB			"-"
  MUL			"*"
  DIV			"/"
  SCOLON  		";"
  LPARENTHESES		"("
  RPARENTHESES		")"
  LBRACE        	"{"
  RBRACE        	"}"
  GREATER       	">"
  LESS          	"<"
  EQUAL         	"=="
  NONEQUAL      	"!="
  ASSIGN        	"="
  WHILE			"while"
  IF			"if"
  PRINT			"print"
  QMARK         	"?(scanf)"
  ERROR
;

%token < double >      NUMBER
%token < std::string > VARIABLE
%token < std::string > WORD
%token TYPE


%nterm < ezg::INode* > act
%nterm < ezg::INode* > declaration_variable
%nterm < ezg::INode* > access_variable
%nterm < ezg::INode* > access_or_declare_variable

%nterm < ezg::INode* > nonscolon_act
%nterm < ezg::INode* > ntif
%nterm < ezg::INode* > ntwhile
%nterm < ezg::INode* > ntscanf

%nterm < ezg::IScope* > scope
%nterm < ezg::IScope* > open_first
%nterm < ezg::IScope* > open_scope
%nterm < std::vector< ezg::INode* > > inside_scope

%nterm < ezg::INode* > condition;

%nterm < ezg::INode* > exprLvl1 exprLvl2 exprLvl3


%left '+' '-'
%left '*' '/'

%start program

%%

program
:   open_first inside_scope     {   $1->insertNode($2);
                                    gScopeStack.pop();
                                }
;


open_first
:   /* empty */     {   $$ = ezg::IScope::make_separate().release();
			driver->insert($$);
                        $$->entry();
                        gScopeStack.push($$);
                    }
;


scope
:   open_scope inside_scope RBRACE      {   $$ = $1;
                                            $$->insertNode($2);
                                            $$->exit();
                                            gScopeStack.pop();
                                        }
|   LBRACE RBRACE                       {   /* empty */  }
;


open_scope
:   LBRACE                              {   	$$ = ezg::IScope::make_inside_current().release();
						driver->insert($$);
                                            	$$->entry();
                                            	gScopeStack.push($$);
                                        }
;


inside_scope
:   inside_scope act SCOLON     {   $$.insert($$.end(), $1.begin(), $1.end());
                                    $$.push_back($2);
                                }
|   inside_scope nonscolon_act  {   $$.insert($$.end(), $1.begin(), $1.end());
                                    $$.push_back($2);
                                }
|   inside_scope scope          {   $$.insert($$.end(), $1.begin(), $1.end());
                                    $$.push_back($2);
                                }
|   error SCOLON		{   }
|   /* empty */			{}
;


act
//:   declaration_variable ASSIGN exprLvl1	{ $$ = ezg::INode::make_assign($1, $3).release();   			driver->insert($$);}
//|   declaration_variable ASSIGN ntscanf	{ $$ = ezg::INode::make_assign($1, $3).release(); 	driver->insert($$);}
:   access_or_declare_variable ASSIGN exprLvl1		{ $$ = ezg::INode::make_assign($1, $3).release(); 			driver->insert($$);}
|   access_or_declare_variable ASSIGN ntscanf		{ $$ = ezg::INode::make_assign($1, $3).release(); 	driver->insert($$);}
|   PRINT exprLvl1				{ $$ = ezg::INode::make_print($2).release(); 				driver->insert($$);}
;


ntscanf
:	QMARK			{	$$ = ezg::INode::make_qmark().release(); 	driver->insert($$); }
;


declaration_variable
:	TYPE VARIABLE		{	auto id = gScopeStack.top()->declareVar($2);
					if (!id.has_value()) {
                                        	throw syntax_error(@1, "multiple definition of variable");

					}
					else {
						$$ = ezg::INode::make_var(id.value()).release();
						driver->insert($$);
					}
				}
;


nonscolon_act
:   ntif                        {   $$ = $1; }
|   ntwhile                     {   $$ = $1; }
;


ntif
:   IF LPARENTHESES condition RPARENTHESES scope        {   $$ = ezg::INode::make_if($3, $5).release(); 	driver->insert($$);}
//|   IF LPARENTHESES condition RPARENTHESES act SCOLON   {   $$ = ezg::INode::make_if($3, $5).release(); 	driver->insert($$);}
;


ntwhile
:   WHILE LPARENTHESES condition RPARENTHESES scope        {   $$ = ezg::INode::make_while($3, $5).release(); 	driver->insert($$);}
//|   WHILE LPARENTHESES condition RPARENTHESES act SCOLON   {   $$ = ezg::INode::make_while($3, $5).release(); 	driver->insert($$);}
;


condition
:   exprLvl1 GREATER  exprLvl1          {   $$ = ezg::INode::make_op(ezg::Operator::Greater,  $1, $3).release();	 driver->insert($$);}
|   exprLvl1 LESS     exprLvl1          {   $$ = ezg::INode::make_op(ezg::Operator::Less,     $1, $3).release();   driver->insert($$);}
|   exprLvl1 EQUAL    exprLvl1          {   $$ = ezg::INode::make_op(ezg::Operator::Equal,    $1, $3).release();   driver->insert($$);}
|   exprLvl1 NONEQUAL exprLvl1          {   $$ = ezg::INode::make_op(ezg::Operator::NonEqual, $1, $3).release();   driver->insert($$);}
;


exprLvl1
:	exprLvl2 ADD exprLvl1           {   $$ = ezg::INode::make_op(ezg::Operator::Add, $1, $3).release();   driver->insert($$);}
| 	exprLvl2 SUB exprLvl1           {   $$ = ezg::INode::make_op(ezg::Operator::Sub, $1, $3).release();   driver->insert($$);}
| 	exprLvl2			{   $$ = $1; }
;


exprLvl2
:	exprLvl3 MUL exprLvl2           {   $$ = ezg::INode::make_op(ezg::Operator::Mul, $1, $3).release();   driver->insert($$);}
| 	exprLvl3 DIV exprLvl2           {   $$ = ezg::INode::make_op(ezg::Operator::Div, $1, $3).release();   driver->insert($$);}
| 	exprLvl3			{   $$ = $1; }
;


exprLvl3
:	LPARENTHESES exprLvl1 RPARENTHESES  	{   $$ = $2;              }
| 	NUMBER				  	{   $$ = ezg::INode::make_val($1).release();    driver->insert($$);}
|	access_variable				{   $$ = $1; }
;


access_variable
:	VARIABLE				{   	auto is_visible = gScopeStack.top()->visible($1);
                                                   	if (is_visible.has_value()) {
                                                        	$$ = ezg::INode::make_var(is_visible.value()).release();
                                                        	driver->insert($$);
                                                        }
                                                        else {
                                                        	throw syntax_error(@1, "undefined variable");
                                                        }
                                                 }


access_or_declare_variable
:	VARIABLE			{	auto is_visible = gScopeStack.top()->visible($1);
                                                if (is_visible.has_value()) {
                                                	$$ = ezg::INode::make_var(is_visible.value()).release();
                                                	driver->insert($$);
                                                }
                                                else {
                                                	auto id = gScopeStack.top()->declareVar($1);
                                                	if (!id.has_value()) {
                                                		throw syntax_error(@1, "cant access or declare of variable");
                                                	}
                                                	else {
                                                		$$ = ezg::INode::make_var(id.value()).release();
                                                		driver->insert($$);
                                                	}
                                                }
					}
;

%%

namespace yy {

	parser::token_type yylex (parser::semantic_type* yylval, parser::location_type* l, Driver* driver) {
		return driver->yylex (l, yylval);
	}

	void parser::error (const parser::location_type& l, const std::string& msg) {
		auto tmp = symbol_type(0, l);
		context ctx(*this, tmp);
		driver->error(ctx, msg);
	}

	void parser::report_syntax_error(parser::context const& ctx) const
	{
		driver->error(ctx, "syntax error");
	}
}