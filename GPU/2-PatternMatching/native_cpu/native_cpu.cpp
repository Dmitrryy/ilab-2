/*************************************************************************************************
 *
 *   native_cpu.cpp
 *
 *   Created by dmitry
 *   16.03.2021
 *
 ***/

#include "native_cpy.hpp"



namespace ezg
{


    size_t countPatternMatches(const std::string& str, const std::string& pattern)
    {
        size_t result = 0;

        size_t cur_pos = str.find(pattern);
        while(cur_pos != std::string::npos)
        {
            result++;
            cur_pos = str.find(pattern, cur_pos + 1);
        }

        return result;
    }



    std::vector< size_t > countPatternMatches(const std::string& str, const std::vector< std::string >& patterns)
    {
        std::vector< size_t > result;
        result.reserve(patterns.size());
        for (auto&& pattern : patterns) {
            result.push_back(countPatternMatches(str, pattern));
        }
        return result;
    }


}//namespace ezg