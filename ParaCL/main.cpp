#include <iostream>
#include <fstream>
#include "parser/driver.h"


int main(int argc, char* argv[])
{
    std::fstream in(argv[1]);
    if (!in.is_open()) {
        std::cerr << "cant open file: " << argv[1] << std::endl;
        return 1;
    }

    auto *lexer = new Scanner;
    lexer->switch_streams(in, std::cout);

    yy::Driver driver { lexer };
    driver.parse ();
    auto n = driver.getData();
    static_cast< ezg::IScope* >(n[0])->execute();

    for (size_t cur = 0; cur < n.size(); cur++) {
        delete static_cast< ezg::INode* >(n[cur]);
    }

    delete lexer;

    return 0;
}