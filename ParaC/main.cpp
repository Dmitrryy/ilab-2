#include <iostream>
#include <fstream>
#include "parser/driver.h"

extern FILE* yyin;

int main(int argc, char* argv[])
{
    std::fstream in("tests/test1.txt");
    if (!in.is_open()) {
        std::cerr << "cant open file: " << argv[1] << std::endl;
        return 1;
    }

    auto *lexer = new Scanner;
    lexer->switch_streams(in, std::cout);

    yy::Driver driver { lexer };
    driver.parse ();
    auto n = (ezg::IScope*)driver.getNode();
    n->execute();

    return 0;
}