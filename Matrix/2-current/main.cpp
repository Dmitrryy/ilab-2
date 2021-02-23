/**************************************************************************************************
 *
 *   main.c
 *
 *   Created by dmitry
 *   ?.?.2020
 *
 ***/

//
/// 1-determinant
///======================================================================================
/// This program calculates the current in each edge of the circuit.
///
/// The input is supplied with the edges of the graph with resistance and EMF(optional).
/// Flex and bison are used for parsing.
/// Example:
/// 1 -- 2, 4.0;
/// 1 -- 3, 10.0;
/// 1 -- 4, 2.0; -12.0
/// 2 -- 3, 60.0;
/// 2 -- 4, 22.0;
/// 3 -- 4, 5.0;
///
/// At the output, the edges are in the same order with the specified current.
/// Example:
/// 1 -- 2: 0.442958 A
/// 1 -- 3: 0.631499 A
/// 1 -- 4: -1.07446 A
/// 2 -- 3: 0.0757193 A
/// 2 -- 4: 0.367239 A
/// 3 -- 4: 0.707219 A
///======================================================================================
///======================================================================================
//



#include "ParserDriver.h"
#include <circuit/Circuit.h>

#include <set>
#include <iostream>


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

