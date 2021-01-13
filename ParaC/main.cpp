#include <iostream>

#include "parser/driver.h"

extern FILE* yyin;

int main(int argc, char* argv[])
{
    freopen("tests/test1.txt", "r", stdin);
/*    if (fd_in == nullptr) {
        std::cerr << "cant open file: " << argv[1] << std::endl;
        return 1;
    }
    yyin = fd_in;*/


    FlexLexer *lexer = new yyFlexLexer;
    yy::Driver driver { lexer };
    driver.parse ();

    auto n = driver.getNode();
    n->execute();

    return 0;
}