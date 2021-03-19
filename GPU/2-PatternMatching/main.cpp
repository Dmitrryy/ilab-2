/*************************************************************************************************
 *
 *   main.cpp
 *
 *   Created by dmitry
 *   16.03.2021
 *
 ***/



#include <fstream>

#include "../../OtherLibs/timer.h"
#include "native_cpu/native_cpy.hpp"
#include "filter_GPU/PatternMatching.hpp"



template< typename T >
std::pair< std::string, std::vector< std::string > > getData2(std::basic_istream< T >& inStream);



int main(int argc, char* argv[])
{
#ifndef NDEBUG
    auto&& inStream = std::ifstream("tests/4.txt");
    assert(inStream.is_open());
    auto&& outStream = std::cout;
#else
    auto&& inStream = std::cin;
    auto&& outStream = std::cout;
#endif

    auto&& data = getData2(inStream);

/*    auto&& result = ezg::countPatternMatches(data.first, data.second);

    for (size_t i = 0, mi = data.second.size(); i < mi; i++) {
        outStream << i << ' ' << result.at(i) << std::endl;
    }*/


    std::vector< cl::Platform > platforms;
    cl::Platform::get(&platforms);
    if(platforms.empty()) {
        throw std::runtime_error("cant find any platform!");
    }
    cl::Device res_device;
    for(auto&& pl : platforms) {
        try {
            std::vector< cl::Device > devices;
            pl.getDevices(CL_DEVICE_TYPE_ALL, &devices);

            for (auto&& dev : devices) {
                if (dev.getInfo< CL_DEVICE_COMPILER_AVAILABLE >()) {
                    res_device = dev;
                    break;
                }
            }
        }
        catch (cl::Error& ex) {
            ex.what();
        }
        if (res_device != cl::Device{}) {
            break;
        }
    }
    if (res_device == cl::Device{}) {
        throw std::runtime_error("cant find any device");
    }

#define MEASUREMENT
#ifndef MEASUREMENT
    ezg::PatternMatchingGPU pmg(res_device);

    auto&& res = pmg.match(data.first, data.second);

    for (size_t i = 0, mi = data.second.size(); i < mi; ++i)
    {
        std::cout << i + 1 << ' ' << res[i].size() << std::endl;
    }
#else
    ezg::PatternMatchingGPU pmg(res_device);

    ezg::Timer timer;
    auto&& res1 = pmg.match(data.first, data.second);
    double timeGPU = timer.elapsed();

    timer.reset();
    auto&& res2 = ezg::countPatternMatches(data.first, data.second);
    double timeCPU = timer.elapsed();

    std::cout << "GPU: " << timeGPU << "sec\n";
    std::cout << "CPU :" << timeCPU << "sec\n";

    for(size_t i = 1, mi = data.second.size(); i < mi; ++i) {
        if(res1[i].size() != res2[i]) {
            std::cerr << "hmmmmmmmmmmmmmmmm\n";
            std::cerr << '[' << i << ']' << res1[i].size() << ' ' << res2[i] << std::endl;
        }
    }
#endif // MEASUREMENT
    return 0;
}




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
    assert(inStream.gcount() == size_str);
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