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

#ifdef DEBUG
    freopen("tests/test1.txt", "r", stdin);
    driver.setFileName("tests/test1.txt");
#endif
    std::istringstream in("");


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

#ifdef DEBUG
    std::cout << circuit.dumpStr() << std::endl;
#endif

    circuit.calculateCurrent();

    auto currents = circuit.getCurrents();

    for (const auto& cur : data)
    {
        std:: cout << cur.v1 << " -- " << cur.v2 << ": " << ((currents.at(cur.v1, cur.v2) == 0) ? 0 : currents.at(cur.v1, cur.v2)) << " A" << std::endl;
    }
}

