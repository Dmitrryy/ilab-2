#include <iostream>

#include "ParserDriver.h"
#include <circuit/Circuit.h>

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
#ifdef DEBUG
    freopen("tests/test1.txt", "r", stdin);
#endif
    FlexLexer* lexer = new yyFlexLexer;
    yy::ParsDriver driver(lexer);

    auto res_pars = driver.parse();
    if (!res_pars) {
        std::cerr << "parsing terminate\n";
        return 1;
    }

    auto data = driver.getData();

    delete lexer;

    ezg::Circuit circuit;
    for (const auto& edge : data) {
        circuit.connect(edge);
    }

#ifdef DEBUG
    std::cout << circuit.dumpStr() << std::endl;
#endif

    circuit.calculateCurrent();

    auto edges = circuit.getData();

    for (auto it = edges.crbegin(); it != edges.crend(); ++it)
    {
        auto cur = *it;
        std:: cout << cur.v1 << " -- " << cur.v2 << ": " << ((cur.current.value() == 0) ? 0 : cur.current.value()) << " A" << std::endl;
    }
}

