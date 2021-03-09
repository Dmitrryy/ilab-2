/*************************************************************************
 *
 * helpa.h
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/

#pragma once

#include <fstream>
#include <vector>
#include <string>


#define DEBUG_MSG(msg) \
(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ' ' + std::string(msg))

namespace vks
{

	std::vector<char> readFile(const std::string& filename);

}//namespace vks