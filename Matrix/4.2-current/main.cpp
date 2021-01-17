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


    circuit.calculateCurrent();

    auto currents = circuit.getCurrents();

    for (const auto& cur : data)
    {
        std:: cout << cur.v1 << " -- " << cur.v2 << ": " << ((std::abs(currents.at(cur.v1, cur.v2)) < matrix::EPSIL) ? 0 : currents.at(cur.v1, cur.v2)) << " A" << std::endl;
    }
}

