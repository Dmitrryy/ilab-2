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
    matrix::Matrix< double > t = {
            {1, 2, -1, 1},
            {2, -1, 2, 2}
    };
    auto solv = t.solve({0.6, 4});
    std::cout << solv.first << solv.second << std::endl;

    return 0;
    //we shoot a cannon at sparrows
    freopen("test.txt", "r", stdin);
    FlexLexer* lexer = new yyFlexLexer;
    yy::ParsDriver driver(lexer);
    driver.parse();

    auto circuit  = driver.getData();
    std::cout << circuit.dumpStr() << std::endl;

    auto cycles = circuit.findCycles();

    for (const auto& c : cycles) {
        int i = 0;
        std::cout << '[' << i << "] ";
        for (const auto& elem : c) {
            std::cout << elem.v1 << ' ' << elem.v2 << ' ';
        }
        std::cout << std::endl;

    }

    circuit.calculateCurrent();

/*    matrix::Matrix< double > LSystem(cycles.size(), graph.getColumns());
    const size_t num_cycles = cycles.size();
    for (size_t c = 0; c < num_cycles; c++)
    {
        double eds = 0;
        const size_t num_vertex = cycles[c].size();
        for (size_t k = 1; k < num_vertex; k++)
        {
            const auto& cur = data.at(cycles[c][k - 1], cycles[c][k]);
            eds += cur.voltage;
            LSystem
        }
    }*/
}

