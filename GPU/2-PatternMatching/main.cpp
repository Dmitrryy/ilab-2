/*************************************************************************************************
 *
 *   main.cpp
 *
 *   Created by dmitry
 *   16.03.2021
 *
 ***/

#include <fstream>

#include "native_cpu/native_cpy.hpp"
#include "tests/gen.hpp"

//#define GEN_TEST

int main()
{
#ifdef GEN_TEST
    auto&& outTest = std::ofstream("tests/2.txt");
    auto&& outAns = std::ofstream("tests/2a.txt");
    ezg::gen_test_pattern_matching(outTest, outAns, 1000, 10, 7);
    return 1;
#endif
    auto&& inStream = std::cin;
    auto&& outStream = std::cout;


    std::string str;
    std::vector< std::string > patterns;


    size_t size_str = 0;
    inStream >> size_str;
    str.reserve(size_str);
    inStream >> str;

    size_t num_patterns = 0;
    inStream >> num_patterns;
    patterns.reserve(num_patterns);
    for (size_t i = 0; i < num_patterns; ++i)
    {
        size_t size_pat = 0;
        inStream >> size_pat;
        std::string cur_pat;
        cur_pat.reserve(size_pat);

        inStream >> cur_pat;

        patterns.emplace_back(std::move(cur_pat));
    }

    auto&& result = ezg::countPatternMatches(str, patterns);

    for (size_t i = 0; i < num_patterns; i++) {
        outStream << i << ' ' << result.at(i) << std::endl;
    }

    return 0;
}