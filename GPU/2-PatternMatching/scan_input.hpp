/*************************************************************************************************
 *
 *   scan_input.hpp
 *
 *   Created by dmitry
 *   20.03.2021
 *
 ***/

#pragma once

#include <vector>
#include <string>
#include <iostream>


namespace ezg
{



    template< typename T >
    std::pair< std::string, std::vector< std::string > > getData2(std::basic_istream< T >& inStream)
    {
        std::string str;
        std::vector< std::string > patterns;


        size_t size_str = 0;
        inStream >> size_str;
        inStream.ignore();
        str.resize(size_str);
        inStream.read(str.data(), size_str);

        inStream.ignore();

        size_t num_patterns = 0;
        inStream >> num_patterns;
        inStream.ignore();
        patterns.reserve(num_patterns);
        for (size_t i = 0; i < num_patterns; ++i)
        {
            size_t size_pat = 0;
            inStream >> size_pat;
            inStream.ignore(1);
            std::string cur_pat;
            cur_pat.resize(size_pat);

            inStream.read(cur_pat.data(), size_pat);
            inStream.ignore(1);

            patterns.emplace_back(std::move(cur_pat));
        }

        return { str, patterns };
    }

}//namespace ezg