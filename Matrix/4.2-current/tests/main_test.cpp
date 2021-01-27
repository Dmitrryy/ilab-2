#include <iostream>

#include "../ParserDriver.h"
#include <circuit/Circuit.h>

#include <set>
#include <vector>
#include <cassert>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace ezg {
    void genTest(size_t x, size_t y, float eds, std::ostream &test_out, std::ostream &ans_out);
}

int main()
{
/*    std::ofstream out ("tests/test9.txt");
    std::ofstream ans ("tests/test9a.txt");

    ezg::genTest(3, 3, 120, out, ans);
    return 0;*/



    auto* lexer = new Scanner;
    yy::ParsDriver driver(lexer);


    freopen("tests/test1.txt", "r", stdin);
    driver.setFileName("tests/test3.txt");

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