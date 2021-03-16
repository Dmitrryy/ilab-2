/*************************************************************************************************
 *
 *   native_cpy.hpp
 *
 *   Created by dmitry
 *   16.03.2021
 *
 ***/
#pragma once


#include <iostream>
#include <vector>
#include <string>



namespace ezg
{

    size_t countPatternMatches(const std::string& str, const std::string& pattern);
    std::vector< size_t > countPatternMatches(const std::string& str, const std::vector< std::string >& patterns);

}//namespace ezg