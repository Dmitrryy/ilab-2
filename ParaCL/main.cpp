#include <iostream>
#include <fstream>
#include "parser/driver.h"


int main(int argc, char* argv[])
{
#ifdef DEBUG
    std::ifstream in("tests/test1.txt");
#else
    std::ifstream in(argv[1]);
#endif
    if (!in) {
        std::cerr << "cant open file: " << argv[1] << std::endl;
        return 1;
    }

    auto *lexer = new Scanner("tests/test1.txt");
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