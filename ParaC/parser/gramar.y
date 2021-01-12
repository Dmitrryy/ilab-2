%language "c++"

%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%param {Driver* driver}

%code requires
{

	#include <algorithm>
	#include <string>
	#include <vector>

	// forward declaration of argument to parser
	namespace yy { class Driver; }

}

%code
{

	#include "driver.h"
    #include <stack>

    namespace ezg {
        class INode;
        class IScope;
    }

	namespace yy {

		parser::token_type yylex (parser::semantic_type* yylval, Driver* driver);

	}

	extern ScopeTable gScopeTable;

}

%token
/* Список токенов */
  ADD 			"+"
  SUB			"-"
  MUL			"*"
  DIV			"/"
  SCOLON  		";"
  LPARENTHESES	"("
  RPARENTHESES	")"
  LBRACE        "{"
  RBRACE        "}"

  GREATER       ">"
  LESS          "<"
  EQUAL         "=="
  NONEQUAL      "!="

  ASSIGN        "="

  WHILE
  IF
  PRINT
  QMARK         "?"

  ERROR
;

%token <double>      NUMBER
%token <std::string> WORD
%token TYPE

/* Объявление нетерминалов */
%nterm < INode* > act //todo

%nterm < INode* > nonscolon_act
%nterm < INode* > ntif
%nterm < INode* > ntwhile

%nterm < IScope* > scope
%nterm < IScope* > open_scope
%nterm < std::vector< INode* > > inside_scope

%nterm < INode* > condition;

%nterm < INode* > exprLvl1 exprLvl2 exprLvl3//todo

/* Левоассоциативные и правоассоциативные лексемы */
%left '+' '-'
%left '*' '/'

%start program

%%

program
:   inside_scope            {   IScope* proga = IScope::make_separate();
                                proga->insertNode($1);
                                driver->setNode(proga);
                            }
|   /* empty */
;


scope
:   open_scope inside_scope RBRACE      {   $$ = $1;
                                            $$->insert($2);
                                            $$->exit();
                                        }
|   LBRACE RBRACE                       {   /* empty */  }
;


open_scope
:   LBRACE                              {   $$ = IScope::make_inside_current();
                                            $$->entry();
                                        }
;


inside_scope
:   inside_scope act SCOLON     {   $$.insert($$.end(), $1.begin(), $1.end());
                                    $$.push_back($2);
                                }
|   inside_scope nonscolon_act  {   $$.insert($$.end(), $1.begin(), $1.end());
                                    $$.push_back($2);
                                }
|   /* empty */
;

//todo
act
:
;

nonscolon_act
:   ntif                        {   $$ = $1; }
|   ntwhile                     {   $$ = $1; }
;


ntif
:   IF LPARENTHESES condition RPARENTHESES scope        {   $$ = INode::make_if($3, $5); }
|   IF LPARENTHESES condition RPARENTHESES act SCOLON   {   $$ = INode::make_if($3, $5); }
;


ntwhile
:   WHILE LPARENTHESES condition RPARENTHESES scope        {   $$ = INode::make_if($3, $5); }
|   WHILE LPARENTHESES condition RPARENTHESES act SCOLON   {   $$ = INode::make_if($3, $5); }
;


condition
:   exprLvl1 GREATER  exprLvl1          {   $$ = INode::make_op(ezg::Operator::Greater,  $1, $3);   }
|   exprLvl1 LESS     exprLvl1          {   $$ = INode::make_op(ezg::Operator::Less,     $1, $3);   }
|   exprLvl1 EQUAL    exprLvl1          {   $$ = INode::make_op(ezg::Operator::Equal,    $1, $3);   }
|   exprLvl1 NONEQUAL exprLvl1          {   $$ = INode::make_op(ezg::Operator::NonEqual, $1, $3);   }
;


exprLvl1
:	exprLvl2 ADD exprLvl1           {   $$ = INode::make_op(ezg::Operator::Add, $1, $3);   }
| 	exprLvl2 SUB exprLvl1           {   $$ = INode::make_op(ezg::Operator::Sub, $1, $3);   }
| 	exprLvl2				        {   $$ = $1; }
;


exprLvl2
:	exprLvl3 MUL exprLvl2           {   $$ = INode::make_op(ezg::Operator::Mul, $1, $3);   }
| 	exprLvl3 DIV exprLvl2           {   $$ = INode::make_op(ezg::Operator::Div, $1, $3);   }
| 	exprLvl3				        {   $$ = $1; }
;


exprLvl3
:	LPARENTHESES exprLvl1 RPARENTHESES  { $$ = $2;              }
| 	NUMBER				  				{ $$ = make_val($1);    }
|	VARIABLE							{ $$ = $1; }//todo
;

////////////////////////////////////////////////////////////////////







program
:   program act SCOLON
|   /* empty */
;





program:
	program exprLvl1 SCOLON	{ std::cout << "expression found: " << $2 << std::endl; }
|
;



%%

namespace yy {

	parser::token_type yylex (parser::semantic_type* yylval, Driver* driver) {
		return driver->yylex (yylval);
	}

	void parser::error (const std::string&) {}

}