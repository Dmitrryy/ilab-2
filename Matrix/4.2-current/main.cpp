#include <iostream>

#include "ParserDriver.h"
#include "../Libs/Trace/trace.h"


#include <set>
#include <vector>
#include <cassert>
#include <algorithm>

std::vector< std::vector< int > > findCycles(const matrix::Matrix< int >& ig, size_t cur, std::vector< int > trace = {});


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

    auto data  = driver.getData();
    auto graph = driver.getIGraph();
    std::cout << graph << std::endl;

    auto cycles = findCycles(graph, 1);

    for (const auto& c : cycles) {
        int i = 0;
        std::cout << '[' << i << "] ";
        for (const auto& elem : c) {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;
    }

    matrix::Matrix< double > LSystem(cycles.size(), graph.getColumns());
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
    }
}


std::vector< std::vector< int > > findCycles(const matrix::Matrix< int >& ig, size_t cur, std::vector< int > trace)
{
    std::vector< std::vector< int > > multi_res;

    const size_t columns = ig.getColumns();
    const size_t lines = ig.getLines();
    for (size_t c = 0; c < columns; c++)
    {
        if (ig.at(cur, c) == 1)
        {
            for (size_t next_v = 0; next_v < lines; next_v++)
            {
                if (ig.at(next_v, c) == 1 && next_v != cur)
                {
                    auto it = std::find(trace.begin(), trace.end(), next_v);
                    if (it == trace.end())
                    {
                        auto tmp_trace = trace;

                        tmp_trace.push_back(cur);
                        auto tmp = findCycles(ig, next_v, std::move(tmp_trace));
                        multi_res.insert(multi_res.end(), tmp.begin(), tmp.end());
                    }
                    else
                    {
                        if (trace.back() != next_v)
                        {
                            //trace.push_back(cur);

                            multi_res.emplace_back(it, trace.end());
                            multi_res.back().push_back(cur);
                        }
                    }
                }

            }
        }

    }

    std::vector< std::vector< int > > res;
    {
        std::set< std::set< int > > set;

        const size_t size = multi_res.size();
        for (size_t i = 0; i < size; i++)
        {
            const size_t old_size = set.size();
            std::set< int > ts;
            set.emplace(multi_res[i].begin(), multi_res[i].end());
            if (old_size < set.size())
            {
                res.emplace_back(std::move(multi_res[i]));
            }
        }
    }

    return res;
}