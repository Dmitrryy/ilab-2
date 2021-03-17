/*************************************************************************************************
 *
 *   gen.hpp
 *
 *   Created by dmitry
 *   16.03.2021
 *
 ***/


#include <iostream>
#include <vector>
#include <string>
#include <fstream>


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
            outAns << i + 1 << ' ' << result.at(i) << std::endl;
        }
    }//gen_test_pattern_matching


    template< typename T >
    static void gen_test_pattern_matching(std::basic_ostream< T >& outTest
                                          , size_t length_str
                                          , size_t num_patterns
                                          , size_t max_length_pattern);
    
}//namespace ezg



int main(int argc, char* argv[])
{
    size_t num_test = 0;
    if (argc != 3) {
        std::cerr << "argc != 3" << std::endl;
        return 1;
    }
    std::string fName_1(argv[1]);
    std::string fName_2(argv[2]);
    auto &&outTest = std::ofstream(fName_1);
    if (!outTest.is_open()) { throw std::runtime_error("can open file: " + fName_1); }
    auto &&outAns  = std::ofstream(fName_2);
    if (!outAns.is_open()) { throw std::runtime_error("can open file: " + fName_2); }

    ezg::gen_test_pattern_matching(outTest, outAns, 100000, 4000, 30);
    return 0;
}