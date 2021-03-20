/*************************************************************************************************
 *
 *   main.cpp
 *
 *   Created by dmitry
 *   16.03.2021
 *
 ***/


//
/// Pattern Matching on GPU
///======================================================================================
/// This is the second level for working with API GPU.
///======================================================================================
///======================================================================================
//


#include <fstream>

#include "scan_input.hpp"
#include "tests_src/gen_test.hpp"
#include "../../OtherLibs/timer.h"
#include "filter_GPU/PatternMatching.hpp"

namespace ezg
{
    void measure_main(const cl::Device &device);
}

int main(int argc, char* argv[])
{
    auto&& inStream = std::cin;
    auto&& outStream = std::cout;


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

#if defined(MEASUREMENT)
    ezg::measure_main(res_device);
#elif defined(GEN_TESTS)
    for(size_t i = 4; i < 8; i++)
    {
        std::ofstream out("tests/" + std::to_string(i) + ".txt");
        ezg::gen_test_pattern_matching(out, 1000000, 500 * i * i, 6, 10);
    }
    for(size_t i = 8; i < 12; i++)
    {
        std::ofstream out("tests/" + std::to_string(i) + ".txt");
        ezg::gen_test_pattern_matching(out, 1000000, 1000, i * i, 2);
    }
    for(size_t i = 12; i < 16; i++)
    {
        std::ofstream out("tests/" + std::to_string(i) + ".txt");
        ezg::gen_test_pattern_matching(out, 1000000, 6000, 6, i * i);
    }
    for(size_t i = 16; i < 20; i++)
    {
        std::ofstream out("tests/" + std::to_string(i) + ".txt");
        ezg::gen_test_pattern_matching(out, i * i * 10000, 6000, 6, 4);
    }
#else
    auto &&data = ezg::getData2(inStream);

    ezg::PatternMatchingGPU pmg(res_device);

    auto &&res = pmg.match(data.first, data.second);

    for (size_t i = 0, mi = data.second.size(); i < mi; ++i)
    {
        outStream << i << ' ' << res[i].size() << std::endl;
    }
#endif
    return 0;
}
