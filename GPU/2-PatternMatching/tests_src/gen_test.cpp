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


#include "gen_test.hpp"

namespace ezg
{

    std::string gen_rand_str(size_t length, Random& rd)
    {
        std::string res;
        res.resize(length);
        for (auto&& c : res) {
            c = static_cast< char >(rd());
        }

        return res;
    }



    int gen_tests_main(int argc, char* argv[])
    {
        size_t num_test = 0;
        if (argc == 3) {
            std::string fName_1(argv[1]);
            std::string fName_2(argv[2]);
            auto &&outTest = std::ofstream(fName_1);
            if (!outTest.is_open()) { throw std::runtime_error("can open file: " + fName_1); }
            auto &&outAns  = std::ofstream(fName_2);
            if (!outAns.is_open()) { throw std::runtime_error("can open file: " + fName_2); }

            ezg::gen_test_pattern_matching(outTest, outAns, 100000, 4000, 30);
        }
        else if (argc == 2) {
            std::string fName_1(argv[1]);
            auto &&outTest = std::ofstream(fName_1);
            if (!outTest.is_open()) { throw std::runtime_error("can open file: " + fName_1); }

            ezg::gen_test_pattern_matching(outTest, 10000000, 6000, 1000, 2);
        }
        return 0;
    }

    
}//namespace ezg
