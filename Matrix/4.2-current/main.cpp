#include <iostream>

#include "ParserDriver.h"

// here we can return non-zero if lexing is not done inspite of EOF detected
int yyFlexLexer::yywrap() {
    return 1;
}

int main()
{
    freopen("test.txt", "r", stdin);
    FlexLexer* lexer = new yyFlexLexer;
    yy::ParsDriver driver(lexer);
    driver.parse();
    driver.printout();
}