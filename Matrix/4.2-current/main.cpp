#include <iostream>

#include "ParserDriver.h"
#include <circuit/Circuit.h>

#include <set>
#include <vector>
#include <cassert>
#include <algorithm>
#include <fstream>
#include <sstream>


int main()
{
    auto* lexer = new Scanner;
    yy::ParsDriver driver(lexer);

    auto res_pars = driver.parse();
    if (!res_pars) {
        std::cerr << "cant pars it =(\n";
        return 1;
    }

    auto data = driver.getData();

    delete lexer;

    ezg::Circuit circuit;
    for (const auto& edge : data) {
        circuit.connect(edge.v1, edge.v2, edge.res, edge.eds);
    }


    try {
        circuit.calculateCurrent();
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what();
        return 1;
    }

    circuit.printCurrents();

    return 0;
}

