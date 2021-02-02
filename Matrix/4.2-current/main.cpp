#include <iostream>

#include "ParserDriver.h"
#include <circuit/Circuit.h>

#include <set>
#include <vector>
#include <cassert>
#include <algorithm>
#include <fstream>
#include <sstream>


//#define GEN_TESTS

#ifdef GEN_TESTS
namespace ezg {
    void genTest(size_t x, size_t y, float eds, std::ostream &test_out, std::ostream &ans_out);
}
#endif


int main()
{
#ifdef GEN_TESTS
    std::ofstream out ("tests/test9.txt");
    std::ofstream ans ("tests/test9a.txt");

    ezg::genTest(3, 3, 120, out, ans);
    return 0;
#endif

#ifdef DEBUG
    freopen("tests/test1.txt", "r", stdin);
#endif

    yy::ParsDriver driver;

    auto res_pars = driver.parse();
    if (!res_pars) {
        std::cerr << "cant pars it =(\n";
        return 1;
    }

    auto data = driver.getData();

    ezg::Circuit circuit;
    for (const auto& edge : data) {
        circuit.connect(edge.v1, edge.v2, edge.res, edge.eds);
    }

#ifdef DEBUG
    circuit.dump(std::cout);
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

