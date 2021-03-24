/*************************************************************************************************
 *
 *   gen_test.hpp
 *
 *   Created by dmitry
 *   20.03.2021
 *
 ***/

#pragma once

#include "../../../OtherLibs/random.h"


namespace ezg
{
    int gen_tests_main(int argc, char* argv[]);

    std::string gen_rand_str(size_t length, Random& rd);

    template< typename T, typename U >
    void gen_test_pattern_matching(std::basic_ostream< T >& outTest
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
            outAns << i + 1 << ' ' << result.at(i) << std::endl;
        }
    }//gen_test_pattern_matching


    template< typename T >
    void gen_test_pattern_matching(std::basic_ostream< T >& outTest
                                          , size_t length_str
                                          , size_t num_patterns
                                          , size_t max_length_pattern
                                          , size_t average_hits)
    {
        ezg::Random rand(-120, 120);
        ezg::Random rand_length(6, max_length_pattern);
        ezg::Random rand_hits(0, average_hits * 2);
        ezg::Random rand_pos(0, length_str);

        std::string str = gen_rand_str(length_str, rand);

        std::vector< std::string > patterns(num_patterns);
        patterns.resize(num_patterns);
        for (size_t i = 0; i < num_patterns; ++i)
        {
            const size_t length = rand_length();
            const std::string cur_pat = gen_rand_str(length, rand);
            patterns.at(i) = cur_pat;

            size_t hints = rand_hits();
            for (size_t k = 0; k < hints; ++k)
            {
                size_t pos = rand_pos();
                while(str.end() - (str.begin() + pos) < length)
                    pos = rand_pos();

                std::copy(cur_pat.cbegin(), cur_pat.cend(), str.begin() + pos);
            }
        }

        outTest << str.size() << std::endl;
        outTest << str << std::endl;

        outTest << num_patterns << std::endl;

        for(const auto& p : patterns) {
            outTest << p.size() << ' ' << p << std::endl;
        }
        return ;
    }

}//namespace ezg