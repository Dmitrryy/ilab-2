#include <iostream>

#include "ParserDriver.h"
#include <circuit/Circuit.h>

#include <set>
#include <vector>
#include <cassert>
#include <algorithm>


int main()
{
    auto* lexer = new Scanner;
    yy::ParsDriver driver(lexer);

#ifdef DEBUG
    freopen("tests/test1.txt", "r", stdin);
    driver.setFileName("tests/test1.txt");
#endif

    auto res_pars = driver.parse();
    if (!res_pars) {
        std::cerr << "cant pars it =(\n";
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

