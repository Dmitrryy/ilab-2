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



template< typename T >
std::pair< std::string, std::vector< std::string > > getData2(std::basic_istream< T >& inStream);



int main(int argc, char* argv[])
{
    auto&& inStream = std::cin;
    auto&& outStream = std::cout;


    auto&& data = getData2(inStream);

    auto&& result = ezg::countPatternMatches(data.first, data.second);

    for (size_t i = 0, mi = data.second.size(); i < mi; i++) {
        outStream << i << ' ' << result.at(i) << std::endl;
    }

    return 0;
}




template< typename T >
std::pair< std::string, std::vector< std::string > > getData2(std::basic_istream< T >& inStream)
{
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

    return { str, patterns };
}