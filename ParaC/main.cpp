#include <iostream>

#include "parser/driver.h"

int main(int argc, char* argv[])
{
    FlexLexer *lexer = new yyFlexLexer;
    yy::Driver driver { lexer };
    driver.parse ();
    return 0;
}