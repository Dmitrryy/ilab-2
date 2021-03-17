/*************************************************************************************************
 *
 *   gtests.cpp
 *
 *   Created by dmitry
 *   16.03.2021
 *
 ***/

#include "../native_cpu/native_cpy.hpp"

#include <fstream>
#include <sstream>
#include <gtest/gtest.h>


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
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


TEST(nativeCPU, test1)
{
    auto&& inStream    = std::ifstream("tests/1.txt");
    auto&& checkStream = std::ifstream("tests/1a.txt");
    auto&& outStream   = std::ostringstream();
    if (!inStream.is_open() || !checkStream.is_open()) { throw std::runtime_error("cant open files."); }

    auto&& data = getData2(inStream);
    auto&& result = ezg::countPatternMatches(data.first, data.second);
    for (size_t i = 0, mi = data.second.size(); i < mi; i++) {
        outStream << i + 1 << ' ' << result.at(i) << std::endl;
    }

    std::ostringstream checkData;
    checkData << checkStream.rdbuf();

    EXPECT_EQ(checkData.str(), outStream.str());
}


TEST(nativeCPU, test2)
{
    auto&& inStream    = std::ifstream("tests/2.txt");
    auto&& checkStream = std::ifstream("tests/2a.txt");
    auto&& outStream   = std::ostringstream();
    if (!inStream.is_open() || !checkStream.is_open()) { throw std::runtime_error("cant open files."); }

    auto&& data = getData2(inStream);
    auto&& result = ezg::countPatternMatches(data.first, data.second);
    for (size_t i = 0, mi = data.second.size(); i < mi; i++) {
        outStream << i + 1 << ' ' << result.at(i) << std::endl;
    }

    std::ostringstream checkData;
    checkData << checkStream.rdbuf();

    EXPECT_EQ(checkData.str(), outStream.str());
}


TEST(nativeCPU, test3)
{
    auto&& inStream    = std::ifstream("tests/3.txt");
    auto&& checkStream = std::ifstream("tests/3a.txt");
    auto&& outStream   = std::ostringstream();
    if (!inStream.is_open() || !checkStream.is_open()) { throw std::runtime_error("cant open files."); }

    auto&& data = getData2(inStream);
    auto&& result = ezg::countPatternMatches(data.first, data.second);
    for (size_t i = 0, mi = data.second.size(); i < mi; i++) {
        outStream << i + 1 << ' ' << result.at(i) << std::endl;
    }

    std::ostringstream checkData;
    checkData << checkStream.rdbuf();

    EXPECT_EQ(checkData.str(), outStream.str());
}


TEST(nativeCPU, test4)
{
    auto&& inStream    = std::ifstream("tests/4.txt");
    auto&& checkStream = std::ifstream("tests/4a.txt");
    auto&& outStream   = std::ostringstream();
    if (!inStream.is_open() || !checkStream.is_open()) { throw std::runtime_error("cant open files."); }

    auto&& data = getData2(inStream);
    auto&& result = ezg::countPatternMatches(data.first, data.second);
    for (size_t i = 0, mi = data.second.size(); i < mi; i++) {
        outStream << i + 1 << ' ' << result.at(i) << std::endl;
    }

    std::ostringstream checkData;
    checkData << checkStream.rdbuf();

    EXPECT_EQ(checkData.str(), outStream.str());
}