/*************************************************************************************************
 *
 *   gen.hpp
 *
 *   Created by dmitry
 *   16.03.2021
 *
 ***/

#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "../../../OtherLibs/random.h"

namespace ezg
{

    static std::string gen_rand_str(size_t length, Random& rd)
    {
        std::string res;
        res.resize(length);
        for (auto&& c : res) {
            c = static_cast< char >(rd());
        }

        return res;
    }
    
    template< typename T, typename U >
    static void gen_test_pattern_matching(std::basic_ostream< T >& outTest
                                   , std::basic_ostream< U >& outAns
                                   , size_t length_str
                                   , size_t num_patterns
                                   , size_t length_pattern)
    {
        length_pattern = std::max(length_pattern, 3lu);
        length_str = std::max(length_pattern, length_str);

        Random letter_rand1('a', 'z');
        Random rend_pattern_num(0, num_patterns - 1);

        std::vector< std::string > patterns;
        patterns.resize(num_patterns);

        for (size_t i = 0; i < num_patterns; ++i)
        {
            auto&& cur_pat = patterns.at(i);
            cur_pat = gen_rand_str(length_pattern, letter_rand1);

            for (size_t k = 0; k < i; ++k){
                if (patterns[k] == cur_pat) {
                    i--;
                    break;
                }
            }

            cur_pat[0] = 'A';
            cur_pat[length_pattern - 1] = 'Z';
        }

        std::vector< size_t > result(num_patterns, 0);
        outTest << length_str << std::endl;
        size_t cur_l = 0;
        while(length_str - cur_l >= length_pattern)
        {
            size_t id = rend_pattern_num();
            outTest << patterns.at(id);

            result.at(id)++;

            cur_l += length_pattern;
        }
        outTest << gen_rand_str(length_str - cur_l, letter_rand1) << std::endl;
        outTest << num_patterns << std::endl;
        for (auto&& p : patterns) {
            outTest << p.size() << ' ' << p << std::endl;
        }


        for (size_t i = 0; i < num_patterns; ++i) {
            outAns << i << ' ' << result.at(i) << std::endl;
        }
    }
    
}//namespace ezg