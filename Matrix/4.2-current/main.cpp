#include <iostream>

#include "ParserDriver.h"


#include <set>
#include <vector>
#include <cassert>
#include <algorithm>

// here we can return non-zero if lexing is not done inspite of EOF detected
int yyFlexLexer::yywrap() {
    return 1;
}

int main()
{
    //we shoot a cannon at sparrows
    freopen("test.txt", "r", stdin);
    FlexLexer* lexer = new yyFlexLexer;
    yy::ParsDriver driver(lexer);
    driver.parse();

    auto circuit  = driver.getData();

#ifdef DEBUG
    std::cout << circuit.dumpStr() << std::endl;

    for (const auto& c : cycles) {
        int i = 0;
        std::cout << '[' << i << "] ";
        for (const auto& elem : c) {
            std::cout << elem.v1 << ' ' << elem.v2 << ' ';
        }
        std::cout << std::endl;

    }
#endif

    circuit.calculateCurrent();

    auto edges = circuit.getData();

    for (const auto& cur : edges)
    {
        std:: cout << cur.v1 << " -- " << cur.v2 << ", " << ((cur.current.value() == 0) ? 0 : cur.current.value()) << " A" << std::endl;
    }
}

